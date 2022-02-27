void myyyinput(FILE * stream, void* extra, char * buf, int *result, size_t max_size) {
  long c = 0x0065;
  int i;
  size_t n = 0;
  long codepointBuffer[4];
  int arrLength;
  int byteLength;

  MAC_YIELD

  while (n < max_size - 16 && c != MYEOF) {
    ((struct qryData*)extra)->getCodepoints(stream,
      codepointBuffer,
      &arrLength,
      &byteLength
    );

    for(i = 0; i < arrLength; i++) {
      c = codepointBuffer[i];

      if(c == MYEOF) {
        break;
      }
      if(c < 0x80) {
        if(c == 0x1a) {
          /* turn soft-EOF to a real one for the benefit of flex */
          ungetc(c, stream);

          c = MYEOF;
          break;
        }

        buf[n++] = (char)c;
      }
      else if(c < 0x800) {
        buf[n++] = ((c >> 6) + 0xC0);
        buf[n++] = ((c & 0x3F) + 0x80);
      }
      else if(c < 0x10000) {
        buf[n++] = ((c >> 12) + 0xE0);
        buf[n++] = (((c >> 6) & 0x3F) + 0x80);
        buf[n++] = ((c & 0x3F) + 0x80);
      }
      else {
        buf[n++] = ((c >> 18) + 0xF0);
        buf[n++] = (((c >> 12) & 0x3F) + 0x80);
        buf[n++] = (((c >> 6) & 0x3F) + 0x80);
        buf[n++] = ((c & 0x3F) + 0x80);
      }
    }
  }

  if(c == MYEOF && ferror(stream)) {
    fputs(TDB_LEX_FAILED, stderr);
    exit(EXIT_FAILURE);
  }

  *result = n;
}
