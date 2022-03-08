void myyyinput(FILE * stream, void* extra, char * buf, int *result, size_t max_size) {
  long c = 0x0065;
  size_t n = 0;
  int byteLength;

  MAC_YIELD

  ((struct qryData*)extra)->inputFileStream = stream;

  while (n < max_size - 16 && c != MYEOF) {
    c = getCurrentCodepoint((struct inputTable*)extra, &byteLength);

    if(c == MYEOF) {
      continue;
    }

    if(c < 0x80) {
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

    getNextCodepoint((struct inputTable*)extra);
  }

  if(c == MYEOF && ferror(stream)) {
    fputs(TDB_LEX_FAILED, stderr);
    exit(EXIT_FAILURE);
  }

  *result = n;
}
