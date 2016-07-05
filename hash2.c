#include "querycsv.h"

#include "hash2.h"

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
      )
    ) {
    return NULL;
  }

  //TODO: check each of the hash maps in turn, and if an entry is found, copy it to RAM and return the copy
  return (struct hash2Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash2,
      5867,
      sizeof(struct hash2Entry),
      &combiningCharCompare
    );
}
