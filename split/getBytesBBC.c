void getBytesBBC(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    /* just cast the codepoint to a byte for ascii control codes and symbols */
    if(codepoint < 0x80 && codepoint != 0x60) {
      *bytes = 0;
      return;
    }

    if(codepoint == 0xa3) { /* £ symbol */
      returnByte = 0x60;  /* normally a backtick, but not on the bbc micro */
      *bytes = &returnByte;
      return;
    }

    returnByte = 0x3f;  /* ascii question mark */
    *bytes = &returnByte;
  }
}
