#include "querycsv.h"
#include "hash2dat.h"

#define DATDEBUG 0
#if DATDEBUG
  char longBuf[12]; /* cater for z88dk's broken varargs */

  #define printfd(...) fprintf(stdout, __VA_ARGS__)
  #define printLong(n) { myltoa(&longBuf, n); fputs(&longBuf, stdout); }
#else
  #define printfd(...)
  #define printLong(n)
#endif

void isInHash2_0(void) {
  int index = (int)(entry.codepoint - 160);

  i = 0;

  if(hash2_[index] == NULL) {
    retval = NULL;
    return;
  }

  while(hash2_[index][i] != 0) {
    codepoints[i] = hash2_[index][i];
    i++;
  }

  entry.length = i;

  retval = &entry;
}

FILE* datafile = NULL;

void openDat(void) {
  char * datName = "qrycsv00.ovl";
  char * path = NULL;
  char * result = NULL;
  char * filename = NULL;

  datafile = fopen(datName, "rb");

  if(datafile != NULL) {
    return;
  }

  result = mystrdup(""); /* Look in the root directory of the current drive */

  /* search the path for the data file if its not found in the current working directory */
  if(datafile == NULL) {
    do {
      d_sprintf(&filename, "%s\\%s", result, datName);
      datafile = fopen(filename, "rb");
      freeAndZero(filename);

      /* z88dk doesn't have getenv. just quit without iterating over the PATH */
      #ifdef __Z88DK
        freeAndZero(result);
        break;
      #else
        if(
          datafile != NULL ||
          (path == NULL && (path = getenv("PATH")) == NULL)
        ) {
          freeAndZero(result);
          break;
        }

        d_strtok(&result, ";", &path);
      #endif
    } while (result != NULL);
  }

  if(datafile == NULL) {
    fprintf(stderr, "Couldn't find %s\n", datName);
    exit(EXIT_FAILURE);
  }
}

void isInHash2_1(void) {
  int32_t lookFor = (int32_t)entry.codepoint;

  unsigned char shortVar;
  unsigned char hasMatch = 0;
  int length;
  int32_t current = 0;

  if(datafile == NULL) {
    openDat();
  }
  else {
    fseek(datafile, 0, SEEK_SET);
  }

  do {
    /* get the current codepoint */
    printfd("1 ");
    printLong(current);
    printfd(" ");

    fread(&current, sizeof(int32_t), 1, datafile);

    printLong(current);
    printfd(" ");
    printLong(lookFor);
    printfd("\n");

    if(current == lookFor) {
      /* skip left value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("2 ");
      printLong(current);
      printfd("\n");

      /* skip right value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("3 ");
      printLong(current);
      printfd("\n");

      /* read length */
      fread(&shortVar, 1, 1, datafile);

      entry.length = (int)shortVar;
      printfd("4 %d\n", entry.length);

      for(length = 0; length != entry.length; length++) {
        fread(&current, sizeof(int32_t), 1, datafile);

        printfd("5 ");
        printLong(current);
        printfd("\n");
        codepoints[length] = current;
      }

      hasMatch = 1;
      break;
    }
    else if(current > lookFor) {
      /* read left value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("6 ");
      printLong(current);
      printfd("\n");

      if(current == -1) {
        /* the value we're looking for isn't in the tree */
        break;
      }
      else {
        fseek(datafile, current, SEEK_SET);
      }
    }
    else {
      /* skip left value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("7 ");
      printLong(current);
      printfd("\n");

      /* read right value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("8 ");
      printLong(current);
      printfd("\n");

      if(current == -1) {
        /* the value we're looking for isn't in the tree */
        break;
      }
      else {
        fseek(datafile, current, SEEK_SET);
      }
    }
  } while (1);

  if(hasMatch) {
    printfd("match length: %d\n", length);

    #if DATDEBUG
      for(i = 0; i < length; i++) {
        printfd("codepoint %d: ", i);
        printLong(codepoints[i]);
        printfd("\n");
      }
    #endif

    retval = &entry;
  }
  else {
    printfd("no match found\n");

    retval = NULL;
  }
}

struct hash2Entry* isInHash2(long codepoint) {
  if(codepoint < 0xA0) {
    return NULL;
  }

  if(codepoint > 0x33FF) {
    if(codepoint < 0xA69C) {
      return NULL;
    }

    if(codepoint > 0xAB5F && codepoint < 0xF900) {
      return NULL;
    }
  }

  entry.codepoint = codepoint;

  if(codepoint < 0x100) {
    isInHash2_0();
  }

  else {
    isInHash2_1();
  }

  return retval;
}
