long getCurrentCodepoint(struct inputTable* table, int* byteLength) {
  if(table) {
    if(byteLength) {
      *byteLength = table->cpByteLength;
    }

    return (table->codepoints)[table->cpIndex];
  }

  if(byteLength) {
    *byteLength = 0;
  }

  return MYEOF;
}
