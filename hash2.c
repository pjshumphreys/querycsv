#include "querycsv.h"

#include "hash2.h"

int longCompare(const void *a, const void *b);

long codepoints[18];
struct hash2Entry entry = {0x0000, 1, &codepoints};

struct hash2Entry* isInHash2(long codepoint) {
  struct hash2Entry* retval;
  int i;

  if(
      codepoint < 0xA0 ||
      (
        codepoint > 0x33FF && (
          codepoint < 0xA69C || (
            codepoint < 0xF900 &&
            codepoint > 0xAB5F
          )
        )
      )
    ) {
    return NULL;
  }

  /* TODO: check each of the hash maps in turn, and if an entry is found, copy it to RAM and return the copy */
  retval = (struct hash2Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash2,
      HASH2SIZE,
      sizeof(struct hash2Entry),
      &longCompare
    );

  if(retval != NULL) {
    entry.codepoint = retval->codepoint;
    entry.length = retval->length;
    
    for(i = 0; i<entry.length; i++) {
      codepoints[i] = retval->codepoints[i];
    }

    retval = &entry;
  }

  return retval;
}

int longCompare(const void *a, const void *b) {
  return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
}
