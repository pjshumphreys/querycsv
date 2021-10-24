void getBytesCP1252(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  struct codepointToByte *lookup;

  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80 || (codepoint > 0x9F && codepoint < 0x100)) {
      *bytes = NULL;
      return;
    }

    if((lookup = (struct codepointToByte*)bsearch(
      (void *)&codepoint,
      (void *)cp1252Bytes,
      SIZE_CP1252BYTES,
      sizeof(struct codepointToByte),
      compareCodepoints
    )) == NULL) {
      returnByte = 0x3f;  /* ascii question mark */
      *bytes = &returnByte;
      return;
    }

    returnByte = lookup->byte;  /* whatever the hash table lookup returned */
    *bytes = &returnByte;
  }
}
