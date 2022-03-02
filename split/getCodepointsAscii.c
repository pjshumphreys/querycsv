/* simple round trip encoding that puts high bit set bytes in the private use area */
void getCodepointsAscii(
    FILE *stream,
    long *codepoints,
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

  if(c < 0x80) {
    codepoints[0] = (long)c;
    return;
  }

  codepoints[0] = (long)c+0xdf80;
}
