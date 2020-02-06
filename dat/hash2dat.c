#include "querycsv.h"
#include "hash2dat.h"

#define printfd(...)

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
  char * path = NULL;
  char * result = NULL;
  char * filename = NULL;

  if(origWd == NULL) {
    datafile = fopen("qrycsv00.ovl", "rb");

    if(datafile != NULL) {
      return;
    }

    result = mystrdup(""); /* Look in the root directory of the current drive */
  }
  else {
    result = mystrdup(origWd);
  }

  /* search the path for the data file if its not found in the current working directory */
  if(datafile == NULL) {
    do {
      d_sprintf(&filename, "%s\\qrycsv00.ovl", result);
      datafile = fopen(filename, "rb");
      freeAndZero(filename);

      /* z88dk can't handle "||" and "&&" in the same expression. */
      /* Just repeat the code for the sake of simplicity */
      #ifdef __Z88DK
      if(datafile != NULL) {
        freeAndZero(result);
        break;
      }
      #endif

      if(
        #ifndef __Z88DK
          datafile != NULL ||
        #endif
        (path == NULL && (path = getenv("PATH")) == NULL)
      ) {
        freeAndZero(result);
        break;
      }

      d_strtok(&result, ";", &path);
    } while (result != NULL);
  }

  if(datafile == NULL) {
    fputs("Couldn't open qrycsv00.ovl\n", stderr);
    exit(EXIT_FAILURE);
  }
}

void isInHash2_1(void) {
  int32_t lookFor = (int32_t)entry.codepoint;

  unsigned char shortVar;
  int32_t current = 0;
  int hasMatch = 0;
  int length;

  if(datafile == NULL) {
    openDat();
  }
  else {
    fseek(datafile, 0, SEEK_SET);
  }

  do {
    /* get the current codepoint */
    printfd("1 %d ", current);
    fread(&current, sizeof(int32_t), 1, datafile);
    printfd("%d %d\n", current, lookFor);

    if(current == lookFor) {
      /* skip left value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("2 %d\n", current);

      /* skip right value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("3 %d\n", current);

      /* read length */
      fread(&shortVar, 1, 1, datafile);
      printfd("4 %d\n", shortVar);

      entry.length = shortVar;

      for(length = 0; length != shortVar; length++) {
        fread(&current, sizeof(int32_t), 1, datafile);

        printfd("5 %d\n", current);
        codepoints[length] = current;
      }

      hasMatch = 1;
      break;
    }
    else if(current > lookFor) {
      /* read left value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("6 %d\n", current);

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
      printfd("7 %d\n", current);

      /* read right value */
      fread(&current, sizeof(int32_t), 1, datafile);
      printfd("8 %d\n", current);

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

    for(i = 0; i < length; i++) {
      printfd("codepoint %d: %x\n", i, codepoints[i]);
    }

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
