#include "querycsv.h"

#include "hash3.h"

int combiningCharCompare(const void *a, const void *b) {
  return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
}

int isCombiningChar(long codepoint) {
  struct hash3Entry * result;

  if(codepoint < 0x300) {
    return 0;
  }

  if(codepoint > 0x309A) {
    if(codepoint < 0xA66F) {
      return 0;
    }

    if(
        codepoint < 0xFB1E &&
        codepoint > 0xABED
    ) {
      return 0;
    }
  }

  result = (struct hash3Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash3EntryMap,
      SIZE_HASH3,
      sizeof(struct hash3Entry),
      &combiningCharCompare
    );

  return (result == NULL ? 0 : result->order);
}
