#include "querycsv.h"
#include "hash2in0.h"

#define printfd(...)

void isInHash2_1(void) {
  int32_t lookFor = (int32_t)entry.codepoint;

  unsigned char shortVar;
  int32_t current = 0;
  int hasMatch = 0;
  int length;

  FILE * fp = fopen("qrycsv00.ovl", "rb");

  if(fp == NULL) {
    fputs("Couldn't open qrycsv00.ovl\n", stderr);
    exit(EXIT_FAILURE);
  }

  for(;;) {
    /* get the current codepoint */
    printfd("1 %d ", current);
    fread(&current, sizeof(int32_t), 1, fp);
    printfd("%d %d\n", current, lookFor);

    if(current == lookFor) {
      /* skip left value */
      fread(&current, sizeof(int32_t), 1, fp);
      printfd("2 %d\n", current);

      /* skip right value */
      fread(&current, sizeof(int32_t), 1, fp);
      printfd("3 %d\n", current);

      /* read length */
      fread(&shortVar, 1, 1, fp);
      printfd("4 %d\n", shortVar);

      entry.length = shortVar;

      for(length = 0; length != shortVar; length++) {
        fread(&current, sizeof(int32_t), 1, fp);

        printfd("5 %d\n", current);
        codepoints[length] = current;
      }

      hasMatch = 1;
      break;
    }
    else if(current > lookFor) {
      /* read left value */
      fread(&current, sizeof(int32_t), 1, fp);
      printfd("6 %d\n", current);

      if(current == -1) {
        /* the value we're looking for isn't in the tree */
        break;
      }
      else {
        fseek(fp, current, SEEK_SET);
      }
    }
    else {
      /* skip left value */
      fread(&current, sizeof(int32_t), 1, fp);
      printfd("7 %d\n", current);

      /* read right value */
      fread(&current, sizeof(int32_t), 1, fp);
      printfd("8 %d\n", current);

      if(current == -1) {
        /* the value we're looking for isn't in the tree */
        break;
      }
      else {
        fseek(fp, current, SEEK_SET);
      }
    }
  }

  fclose(fp);

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
  if(
      codepoint < 0xA0 ||
      (
        codepoint > 0x33FF && (
          codepoint < 0xA69C || (
            codepoint < 0xF900 &&
            codepoint > 0xAB5F
          )
        )
      ) ||
      ((entry.codepoint = codepoint) && FALSE)
    ) {
    return NULL;
  }

  else if(codepoint < 0x100) {
    isInHash2_0();
  }

  else {
    isInHash2_1();
  }

  return retval;
}
