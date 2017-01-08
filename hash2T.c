#include "querycsv.h"

long codepoints[18];
struct hash2Entry entry = {0x0000, 1, &codepoints};
struct hash2Entry* retval;
int i;

DEFINES

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

BLOCKS  return retval;
}
