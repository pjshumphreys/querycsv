
void getBytesUtf16Le(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  unsigned QCSV_SHORT highSurrogate;
  unsigned QCSV_SHORT lowSurrogate;

  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    if(codepoint < 0x10000) {
      *byteLength = 2;

      (*bytes)[0] = codepoint & 0xFF;
      (*bytes)[1] = (codepoint & 0xFF00) >> 8;
    }
    else {
      *byteLength = 4;
      codepoint -= 0x10000;

      lowSurrogate = (codepoint & 0x3FF) + 0xDC00;
      highSurrogate = ((codepoint & 0xFFC00) >> 10) + 0xD800;

      (*bytes)[0] = highSurrogate & 0xFF;
      (*bytes)[1] = (highSurrogate & 0xFF00) >> 8;
      (*bytes)[2] = lowSurrogate & 0xFF;
      (*bytes)[3] = (lowSurrogate & 0xFF00) >> 8;
    }
  }
}
