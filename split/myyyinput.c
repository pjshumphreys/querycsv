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
          c = MYEOF;
          /*soft EOF. */
          #ifdef __Z88DK
            /* fgetc on z88dk seems to behave oddly, but as it doesn't have
               ferror (I implemented it as a macro) we can just close the
               file instead to return EOF from now on */
            fclose(stream);
          #else
            /* Eat up the rest of the file */
            while(fgetc(stream) != EOF) {}
          #endif

          break;
        }

        logNum(c);
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
