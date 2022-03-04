void getBytesUtf32Be(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 4;

    (*bytes)[3] = codepoint & 0xFF;
    (*bytes)[2] = (codepoint >> 8) & 0xFF;
    (*bytes)[1] = (codepoint >> 16) & 0xFF;
    (*bytes)[0] = (codepoint >> 24) & 0xFF;
  }
}
