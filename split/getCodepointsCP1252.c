#include "querycsv.h"

#include "cp1252.h"

void getCodepointsCP1252(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
  ) {
  getCodepoints8Bit(
    stream,
    codepoints,
    arrLength,
    byteLength,
    cp1252
  );
}
