/* simple round trip encoding that puts high bit set bytes in the private use area */
void getCodepointsAscii(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c;

  MAC_YIELD

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  *arrLength = *byteLength = 1;

  if(c < 0x80) {
    codepoints[0] = (long)c;
    return;
  }

  if((c = fgetc(stream)) == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  codepoints[0] = (long)c+0xdf80;
}
