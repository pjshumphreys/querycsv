long getCurrentCodepoint(struct inputTable* table, int* byteLength) {
  long retval;

  if(table) {
    if(byteLength) {
      *byteLength = table->cpByteLength;
    }

    retval = (table->codepoints)[table->cpIndex];

    if(retval == 0x1a) {
      return MYEOF;
    }

    /* prevent unicode surrogates being unescaped */
    if(retval > 0xD7FF && retval < 0xE000) {
      return 0xFFFD;
    }

    return retval;
  }

  if(byteLength) {
    *byteLength = 0;
  }

  return MYEOF;
}
