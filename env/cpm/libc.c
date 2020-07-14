/* fake program to get the necessary libc functions into 1 memory page */
#pragma output noprotectmsdos

/* not really nofileio, but we want to locate __sgoiblk ourselves to make
it the same value for both the fcb and msx2 variants */
#pragma output CRT_ENABLE_STDIO = 0

#define QCSV_NOZ80MALLOC
#include "../../querycsv.h"
#include <fcntl.h>
extern int newline;

void dosload(int pageNumber) __z88dk_fastcall {
  const char * filename = "qrycsv00.ovl";

  static int temp;

  sprintf(filename + 6, "%02d", pageNumber);
  filename[8] = '.';

  if((temp = open(filename, O_RDONLY, 0)) == -1) {
    fputs_z80("Couldn't open ", stderr);
    fputs_z80(filename, stderr);
    exit(EXIT_FAILURE);
  }

  read(temp, (void *)16384, 16384);
  close(temp);
}

/* don't bother to copy the mode parameter as for our purposes
it will always be a static string */
FILE * fopen_z80(const char * filename, const char * mode) {
  return fopen(filename, mode);
}

void free_z80(void *addr) {
  if(addr == NULL) {
    return;
  }

  current = ((struct heapItem *)(addr - sizeof(struct heapItem)));
  current->type = HEAP_FREE;

  /* try to keep the next available free block as unfragmented as possible */
  if(current->next != myHeap.nextFree) {
    return;
  }

  if(NULL == myHeap.nextFree) {
    return;
  }

  next = (struct heapItem *)(addr + current->size);

  if(next != myHeap.nextFree) {
    return;
  }

  current->next = next->next;
  current->size += next->size + sizeof(struct heapItem);
  myHeap.nextFree = current;
}

int fputs_z80(const char * ptr, FILE * stream) {
  int tot;

  if(stream != stdout && stream != stderr) {
    return fputs(ptr, stream);
  }

  tot = strlen(ptr);

  if(newline) {
    fputc_cons('\n');
    newline = FALSE;
  }

  if(ptr[tot-1] == '\n') {
    newline = TRUE;
    ptr[tot-1] = '\0';
  }

  while(*ptr) {
    fputc_cons(*ptr++);
  }

  if(newline) {
    *ptr = '\n';
  }

  return tot;
}

/* fprintf_z80/d_sprintf unified with variadic macros to save space */
int fprintf_z80(int type, void * output, char *format, ...) __stdc {
  size_t newSize;
  char *newStr;
  va_list args;

  /* Check sanity of inputs */
  if(format == NULL) {
    return FALSE;
  }

  newStr = NULL;

  /* get the space needed for the new string */
  va_start(args, format);
  newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
  va_end(args);

  /* Create a new block of memory with the correct size rather than using realloc */
  /* as any old values could overlap with the format string. quit on failure */
  if((newStr = (char*)malloc_z80(newSize + 1)) == NULL) {
    return FALSE;
  }

  /* do the string formatting for real. */
  va_start(args, format);
  vsnprintf(newStr, newSize + 1, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = '\0';

  if(type) {
    fputs_z80(newStr, (FILE *)output);
    free_z80(newStr);
    return newSize;
  }

  (char **)(*output) = newStr;
  return TRUE;
}

void *malloc_z80(unsigned int size) {
  unsigned int cleanedUp;
  unsigned int temp;

  cleanedUp = FALSE;

  /* for larger allocations, try to overlay an exact match starting from the beginning of the heap */
  if(size > 255) {
    current = myHeap.first;

    do {
      if(current == NULL) {
        break;
      }

      if(current == myHeap.nextFree) {
        break;
      }

      if(current->type == HEAP_FREE && current->size == size) {
        current->type = HEAP_ALLOCED;

        return (void *)current + sizeof(struct heapItem);
      }

      current = current->next;
    } while(1);
  }

  temp = size + sizeof(struct heapItem);

  do {
    /* no free memory available. just quit */
    if(myHeap.nextFree == NULL) {
      return NULL;
    }

    /* find a suitable location the put the new data, starting at myHeap.nextFree */
    current = myHeap.nextFree;

    do {
      if(current->type == HEAP_FREE && current->size >= temp) {
        /* suitable location found, set up the headers then return the pointer */
        next = (struct heapItem *)((void*)current + temp);
        next->next = current->next;
        next->size = current->size - temp;
        next->type = HEAP_FREE;

        current->size = size;
        current->type = HEAP_ALLOCED;
        current->next = next;

        myHeap.nextFree = next;

        return (void *)current + sizeof(struct heapItem);
      }

      current = current->next;
    } while(current);

    /* if no suitable free position was found and the heap has already been cleaned up then fail */
    if(cleanedUp) {
      return NULL;
    }

    myHeap.nextFree = NULL;
    current = myHeap.first;

    /* Attempt to coalesce the free blocks together then try again, but only once */
    while(current != NULL) {
      next = current->next;

      if(current->type == HEAP_FREE) {
        if(myHeap.nextFree == NULL) {
          myHeap.nextFree = current;
        }

        if(
          next && next->type == HEAP_FREE &&
          (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + current->size) == next
        ) {
          current->next = next->next;
          current->size += next->size + sizeof(struct heapItem);
          continue;
        }
      }

      current = next;
    }

    cleanedUp = TRUE;
  } while (1);
}

void *realloc_z80(void *p, unsigned int size) {
  void * newOne;
  unsigned int tempSize;
  unsigned int updateNextFree;

  /* if realloc'ing a null pointer then just do a malloc */
  if(p == NULL) {
    return malloc_z80(size);
  }

  if(size == 0) {
    free_z80(p);
    return NULL;
  }

  current = (struct heapItem *)(p - sizeof(struct heapItem));

  next = current->next;

  /* Is the existing block adjacent to a free one with enough
    total space? if so then just resize it and return the existing block */
  if(
    next != NULL &&
    next->type == HEAP_FREE &&
    (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + current->size) == next
  ) {
     /* get the total amount of memory available in this interval */
    tempSize = current->size + next->size;

    if(tempSize >= size) {
      tempSize -= size;

      /* if the nextFree block is the same one as the free one we're updating, update the pointer as well */
      updateNextFree = (next == myHeap.nextFree);

      /* remove the old free block from the linked list as we'll be making a new one */
      current->next = next->next;

      /* update the current block's size to its new value */
      current->size = size;

      next = (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + size);
      next->next = current->next;
      next->size = tempSize;
      next->type = HEAP_FREE;

      current->next = next;

      if(updateNextFree) {
        myHeap.nextFree = next;
      }

      return p;
    }
  }

  /* attempt to allocate a new block of the necessary size, memcpy the data into it then free the old one */
  newOne = malloc_z80(size);

  /* if the malloc failed, just fail here as well */
  if(!newOne) {
    return NULL;
  }

  /* memcpy the data if necessary */
  tempSize = size;

  if(tempSize > current->size) {
    tempSize = current->size;
  }

  if(tempSize) {
    memcpy(newOne, p, tempSize);
  }

  /* free the old data */
  current->type = HEAP_FREE;

  /* return a pointer to the new data */
  return newOne;
}

void reallocMsg(void **mem, size_t size) {
  void *temp;

  if(mem != NULL) {
    if(size) {
      temp = realloc_z80(*mem, size);

      if(temp == NULL) {
        fputs_z80(TDB_MALLOC_FAILED2, stderr);
        exit(EXIT_FAILURE);
      }

      *mem = temp;
    }
    else {
      free_z80(*mem);
      *mem = NULL;
    }
  }
  else {
    fputs(TDB_INVALID_REALLOC, stderr);
    exit(EXIT_FAILURE);
  }
}

void b(char * string) {
  double d;

  FILE* test;
  int num;
  unsigned long num2;

  num = atol(string);
  ftoa(origWd, num, d);
  ltoa(num2, origWd, num);

  abs(num);
  strcpy(string, origWd);
  strncpy(string, origWd, 3);
  num = strcmp(origWd, string);
  num = stricmp(origWd, string);
  num = strncmp(origWd, string, 3);
  num = strnicmp(origWd, string, 3);
  num = strlen(string);
  string = strstr(string, origWd);

  memset(string, 0, 4);
  strcat(string, origWd);
  strncat(string, origWd, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);

  fseek(test, 9, SEEK_SET);
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fputc(num, stderr);
  fflush(stdout);
  isspace(num);
  isdigit(num);
}
