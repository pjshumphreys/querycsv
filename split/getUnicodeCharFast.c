/* getUnicodeCharFast does not check for invalid or overlong bytes. */
/* it also presumes the the entire string is already in nfd form */
QCSV_LONG getUnicodeCharFast(unsigned char *temp, int *bytesMatched) {
  MAC_YIELD

  if(*temp < 0x80) {
    /* return the information we obtained */
    *bytesMatched = 1;

    return (QCSV_LONG)(*temp);
  }
  else if(*temp < 0xE0) {
    /* read 2 bytes */
    *bytesMatched = 2;

    return (QCSV_LONG)((*(temp) << 6) + *(temp+1)) - 0x3080;
  }
  else if(*temp < 0xF0) {
    /* read 3 bytes */
    *bytesMatched = 3;

    return ((QCSV_LONG)(*temp) << 12) + ((QCSV_LONG)(*(temp+1)) << 6) + (QCSV_LONG)(*(temp+2)) - 0xE2080;
  }

  /* read 4 bytes */
  *bytesMatched = 4;

  return ((QCSV_LONG)(*temp) << 18) + ((QCSV_LONG)(*(temp+1)) << 12) + ((QCSV_LONG)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
}
