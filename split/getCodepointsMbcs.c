void getCodepointsMbcs(
    FILE *stream,
    QCSV_LONG *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c;
  int byteIndex = 0;
  struct lookup ** result = NULL;

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  memset(mbcs_temp, 0, mbcs_size);

  *arrLength = *byteLength = 1;

  for(byteIndex = 0; byteIndex < mbcs_trailing; byteIndex++) {
    if((c = fgetc(stream)) == EOF) {
      codepoints[0] = MYEOF;
      return;
    }

    mbcs_temp->bytes[mbcs_trailing - 1] = (unsigned char)c;

    result = bsearch(
      (void*)(&mbcs_temp),
      (void*)b2c,
      mbcs_length,
      sizeof(struct lookup *),
      sortBytes
    );

    if(result != NULL) {
      codepoints[0] = (QCSV_LONG)((*result)->codepoint);
      return;
    }

    *byteLength = *byteLength + 1;

    memmove(mbcs_temp, ((unsigned char *)mbcs_temp) + 1, mbcs_size - 1);
  }

  codepoints[0] = (QCSV_LONG)0xFFFD;
}
