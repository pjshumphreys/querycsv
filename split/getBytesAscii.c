void getBytesAscii(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    /* just cast the codepoint to a byte for ascii control codes and symbols */
    if(codepoint < 0x80) {
      *bytes = 0;
      return;
    }

    returnByte = 0x3f;  /* ascii question mark */
    *bytes = &returnByte;
  }
}
