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

    return retval;
  }

  if(byteLength) {
    *byteLength = 0;
  }

  return MYEOF;
}
