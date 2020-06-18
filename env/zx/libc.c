/* fake program to get the necessary libc functions into 1 memory page */
#define QCSV_NOZ80MALLOC
#include "querycsv.h"

void myexit(int status);

const char pageBuf[256];  /* not really a constant but a buffer that exists in high memory that's only used internally to the libc functions */

__asm
  EXTERN mypager
  EXTERN defaultBank
  EXTERN current
  EXTERN myHeap
__endasm;

/* don't bother to copy the mode parameter as for our purposes it will always be a static string */
FILE * fopen_z80(const char * filename, const char * mode) {
  int len;

  len = strlen(filename);

  if(len > 255) {
    return NULL;  /* very long filenames are not supported. 256 bytes strings should be big enough though */
  }

  memcpy(&pageBuf, filename, len + 1);
  return fopen(&pageBuf, mode);
}

size_t fread_zx(void * ptr, size_t size, size_t count, FILE * stream) {
  int tot2;
  union {
    int tot;
    struct {
      unsigned char remainder;  /* l */
      unsigned char loop;  /* h */
    } bytes;
  } temp;

  if(stream == stdin) {
    return fread(ptr, size, count, stream);
  }

  temp.tot = size * count;
  tot2 = 0;

  while(temp.bytes.loop) {
    tot2 += fread(&pageBuf, 1, 256, stream);

    /* page out esxdos */
    __asm
      push af
      ld a, (defaultBank)
      call mypager
      pop af
    __endasm;

    memcpy(ptr, &pageBuf, 256);

    ptr += 256;
    --(temp.bytes.loop);
  }

  tot2 += fread(&pageBuf, 1, (int)(temp.bytes.remainder), stream);

  /* page out esxdos */
  __asm
    push af
    ld a, (defaultBank)
    call mypager
    pop af
  __endasm;

  memcpy(ptr, &pageBuf, (int)(temp.bytes.remainder));

  return tot2;
}

size_t fwrite_zx(const void * ptr, size_t size, size_t count, FILE * stream) {
  int tot2;
  union {
    int tot;
    struct {
      unsigned char remainder;
      unsigned char loop;
    } bytes;
  } temp;

  temp.tot = size * count;

  /* if stream is stdout or stderr then just call fputc_cons ourselves.
  There are probably reasons why this wouldn't be correct in all general
  cases but it should work well enough for the purposes of this program */
  if(stream == stdout || stream == stderr) {
    tot2 = temp.tot;

    while(temp.tot--) {
      fputc_cons(*ptr++);
    }

    return tot2;
  }

  tot2 = 0;

  while(temp.bytes.loop) {
    memcpy(&pageBuf, ptr, 256);
    tot2 += fwrite(&pageBuf, 1, 256, stream);

    /* page out esxdos */
    __asm
      push af
      ld a, (defaultBank)
      call mypager
      pop af
    __endasm;

    ptr += 256;
    --(temp.bytes.loop);
  }

  memcpy(&pageBuf, ptr, (int)(temp.bytes.remainder));
  tot2 += fwrite(&pageBuf, 1, (int)(temp.bytes.remainder), stream);

  /* page out esxdos */
  __asm
    push af
    ld a, (defaultBank)
    call mypager
    pop af
  __endasm;

  return tot2;
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

/* fprintf_zx/d_sprintf unified with variadic macros to save space */
int fprintf_z80(int type, void * output, char *format, ...) __stdc {
  size_t newSize;
  char *newStr;
  va_list args;

  /* Check sanity of inputs */
  if(format == NULL) {
    return FALSE;
  }

  /* if the stream is stdout or stderr just do a normal printf as the esxdos
    paging won't come into play therefore we don't need to do the formatting twice */
  if(type) {
    if(stdout == (FILE *)output || stderr == (FILE *)output) {
      va_start(args, format);
      newSize = (size_t)(vfprintf((FILE *)output, format, args));
      va_end(args);

      return newSize;
    }
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

  if(type) {
    fwrite_zx(newStr, 1, newSize, (FILE *)output);

    free_z80(newStr);

    return newSize;
  }
  else {
    newStr[newSize] = '\0';

    (char **)(*output) = newStr;

    return TRUE;
  }
}

int fputs_zx(const char * str, FILE * stream) {
  return fwrite_zx(str, 1, strlen(str), stream);
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
        fwrite_zx(TDB_MALLOC_FAILED2, 1, 33, stderr);
        myexit(EXIT_FAILURE);
      }

      *mem = temp;
    }
    else {
      free_z80(*mem);
      *mem = NULL;
    }
  }
  else {
    fputs_zx(TDB_INVALID_REALLOC, stderr);
    myexit(EXIT_FAILURE);
  }
}

void b(char * string, unsigned char * format, ...) {
  va_list args;
  va_list args2;
  double d;

  FILE* test;
  int num;
  unsigned long num2;

  num = atol(string);
  ftoa(origWd, num, d);
  ltoa(num2, origWd, num);

  /* string = malloc(1); */
  /* free(string); */
  /* string = calloc(1, 3); */
  /* string = realloc(string, 5); */
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

  /* fprintf(test, origWd, 1); */
  /* fputs(origWd, test); */

  /* test = fopen(origWd, "rb"); */
  fseek(test, 9, SEEK_SET);
  clearerr(test);
  num = fclose(test);
  /* fread(string, 2, 2, stdin); */
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  /* fwrite(string, 1, 1, stdout); */
  fputc(num, stderr);
  /* fflush(stdout); */
  sprintf(string, origWd, num);
  isspace(num);
  isdigit(num);

  fprintf(stdout, string, format, args);

  va_start(args, format);
  vfprintf(string, format, args);
  va_end(args);

  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);

  /* free(string); */
}

int main(int argc, char * argv[]) {
  /*
    mallinit();
    sbrk(24000, 4000);
  */

  origWd = "%d";
  b(origWd, (unsigned char *)origWd);

  return 0;
}
