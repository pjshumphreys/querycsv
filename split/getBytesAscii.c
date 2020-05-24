void getBytesAscii(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  unsigned char * temp;
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    /* just cast the codepoint to a byte for ascii control codes and symbols */
    if(codepoint < 0x80) {
      *bytes = 0;
      return;
    }

    if(codepoint < 0xe000 || codepoint > 0xe07f) {
      returnByte = 0x3f;  /* ascii question mark */
    }
    else {
      temp = (unsigned char *)(&returnByte);

      /* cast private use area into high bit set bytes */
      *temp = (unsigned char)(codepoint-0xdf80);
    }

    *bytes = &returnByte;
  }
}
