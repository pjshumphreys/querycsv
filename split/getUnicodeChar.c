#include "querycsv.h"

/* this version of getUnicodeChar doesn't check for invalid bytes
  or overlong codepoints, as getCodepoints will have already converted
  them to valid utf-8 bytes within main memory */
long getUnicodeChar(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    void (*get)()
  ) {

  struct hash2Entry* entry = NULL;
  int bytesread = 0;
  long codepoint;
  unsigned char *temp = (unsigned char *)((*offset) + plusBytes);

  MAC_YIELD

  /* if the current byte offset is a utf-8 character
    that's not decomposable then return it */
  if(*temp < 0x80) {
    /* read 1 byte. no checks needed as a 1 byte code
      is never a combining character */
    *bytesMatched = 1;

    return (long)(*temp);
  }
  else if(*temp < 0xE0) {
    /* read 2 bytes */
    codepoint = (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
    if((entry = isInHash2(codepoint))) {
      bytesread = 2;
    }
    else {
      /* otherwise return it */
      *bytesMatched = 2;

      return codepoint;
    }
  }
  else if (*temp < 0xF0) {
    /* read 3 bytes */
    codepoint = ((long)(*(temp)) << 12) + ((long)(*(temp+1)) << 6) + (*(temp+2)) - 0xE2080;

    /* the codepoint is valid. but is it decomposable? */
    if((entry = isInHash2(codepoint))) {
      bytesread = 3;
    }
    else {
      /* otherwise return it */
      *bytesMatched = 3;

      return codepoint;
    }
  }
  else {
    /* read 4 bytes */
    codepoint = (((long)(*temp)) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;

    /* the codepoint is valid. but is it decomposable? */
    if((entry = isInHash2(codepoint))) {
      bytesread = 4;
    }
    else {
      /* otherwise return it */
      *bytesMatched = 4;

      return codepoint;
    }
  }

  /* decomposable codepoints mean the string needs to be NFD normalized. */
  return normaliseAndGet(offset, str, plusBytes, bytesMatched, bytesread, entry);
}
