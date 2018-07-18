void getNextCodepoint(struct inputTable* table) {
  void (*getCodepoints)(FILE *, long *, int *, int *);
  long temp;

  if(table->cpIndex == (table->arrLength) - 1) {
    getCodepoints = chooseGetter(table->fileEncoding);

    /* we've exhausted the current buffer. Get some more codepoints */
    (table->getCodepoints)(
        table->fileStream,
        &(table->codepoints),
        &(table->arrLength),
        &(table->byteLength)
      );

    (table->cpIndex) == 0;
  }
  else {
    (table->cpIndex)++;
  }

  /*the last codepoint in the buffer. We need to ensure the cpByteLength is correct */

  switch(table->fileEncoding) {
    case ENC_UTF8: {
      if(table->arrLength == 1) {
        table->cpByteLength = table->byteLength;
      }
      else if(table->byteLength > 0) {
        table->cpByteLength = 1;
        table->byteLength -= 1;
      }
      else {
        table->cpByteLength = 0;
      }
    } break;

    case ENC_UTF16LE:
    case ENC_UTF16BE: {
      temp = table->codepoints[table->cpIndex];

      if(temp != MYEOF && temp < 0x10000 && table->byteLength > 1) {
        table->cpByteLength = 2;
        table->byteLength -= 2;
      }
      else {
        table->cpByteLength = table->byteLength;
        table->byteLength = 0;
      }
    } break;

    case ENC_UTF32LE:
    case ENC_UTF32BE: {
      temp = table->codepoints[table->cpIndex];

      if(temp != MYEOF && table->byteLength > 3) {
        table->cpByteLength = 4;
        table->byteLength -= 4;
      }
      else {
        table->cpByteLength = table->byteLength;
        table->byteLength = 0;
      }
    } break;

    default:
      if(table->byteLength > 0) {
        table->cpByteLength = 1;
        table->byteLength -= 1;
      }
      else {
        table->cpByteLength = 0;
      }
    break;
  }
}
