void getBytesCP850(
    long codepoint,
    char** bytes,
    int* byteLength
) {
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80) {
      *bytes = NULL;
      return;
    }

    *bytes = getBytesCommon(codepoint, 1);
  }
}
