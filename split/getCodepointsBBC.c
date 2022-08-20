void getCodepointsBBC(
    FILE *stream,
    QCSV_LONG *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c;

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  *arrLength = *byteLength = 1;

  if((c = fgetc(stream)) == EOF) {
    *byteLength = 0;
    codepoints[0] = MYEOF;
    return;
  }

  if(c == 96) {
    codepoints[0] = 163;
    return;
  }

  codepoints[0] = (QCSV_LONG)c;
}
