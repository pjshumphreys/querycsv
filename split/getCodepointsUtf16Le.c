void getCodepointsUtf16Le(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c,c2;

  unsigned QCSV_SHORT highSurrogate;
  unsigned QCSV_SHORT lowSurrogate;

  MAC_YIELD

  *byteLength = 0;

  if(stream == NULL) {
    *arrLength = 0;
    return;
  }

  if(
    (((c = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE)) ||
    (((c2 = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE))
  ) {
    *arrLength = 1;
    codepoints[0] = MYEOF;
    return;
  }

  highSurrogate = (c2 << 8) + c;

  if(highSurrogate < 0xD800 || highSurrogate > 0xDBFF) {
    *arrLength = 1;
    codepoints[0] = (long)highSurrogate;
    return;
  }

  if(
    (((c = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE)) ||
    (((c2 = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE))
  ) {
    *arrLength = 2;

    codepoints[0] = 0xFFFD;
    codepoints[1] = MYEOF;
    return;
  }

  lowSurrogate = (c2 << 8) + c;

  if(lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) {
    *arrLength = 2;
    codepoints[0] = 0xFFFD;
    codepoints[1] = (long)lowSurrogate;
    return;
  }

  *arrLength = 1;
  codepoints[0] = 0x10000 + ((((long)highSurrogate) - 0xD800) << 10) + (((long)lowSurrogate) - 0xDC00);
}
