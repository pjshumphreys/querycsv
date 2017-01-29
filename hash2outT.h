#include "querycsv.h"

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
