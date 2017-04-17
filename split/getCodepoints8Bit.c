#include "querycsv.h"

void getCodepoints8Bit(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength,
    long const *map
  ) {
  int c;

  MAC_YIELD

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  *arrLength = *byteLength = 1;

  if((c = fgetc(stream)) == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  if(c < 0x80) {
    codepoints[0] = (long)c;
    return;
  }

  codepoints[0] = map[c-0x80];
}
