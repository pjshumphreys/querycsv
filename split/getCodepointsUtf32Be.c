void getCodepointsUtf32Be(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c = 0;
  int c2 = 0;
  int c3 = 0;
  int c4 = 0;

  *byteLength = 0;

  if(stream == NULL) {
    *arrLength = 0;
    return;
  }

  *arrLength = 1;

  if(
    (((c = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE)) ||
    (((c2 = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE)) ||
    (((c3 = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE)) ||
    (((c4 = fgetc(stream)) == EOF) || ((*byteLength += 1) && FALSE))
  ) {
    if(c != EOF) {
      *arrLength = 2;
      codepoints[0] = 0xFFFD;
      codepoints[1] = MYEOF;
    }
    else {
      codepoints[0] = MYEOF;
    }

    return;
  }

  codepoints[0] = (((long)c4)) + (((long)c3) << 8) + (((long)c2) << 16) + (((long)c) << 24);
}
