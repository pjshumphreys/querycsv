char *d_charsetEncode(char* s, int encoding, size_t *bytesStored, struct qryData *query) {
  char * buffer = NULL;
  long codepoint;
  int i;
  int byteLength;
  int bytesMatched;
  int bytesToStore;
  char utf16Bytes[4];
  char* bytes = (char *)utf16Bytes; /* a pointer to constant data *OR* some allocated data for utf-16 */
  size_t temp;

  MAC_YIELD

  /* if we don't care how long the returned string is. always includes trailing null byte */
  if(bytesStored == NULL) {
    temp = 0;

    do {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength, encoding);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL ? ((char)codepoint) : bytes[i], &buffer, &temp);
      }

      if(codepoint == 0) {
        return buffer;
      }

      s += bytesMatched;
    } while(1);
  }

  /* if bytesStored is initially non zero, it means limit to read at most that
   * many characters from the source. a trailing null byte isn't added */
  if(*bytesStored != 0) {
    bytesToStore = (int)*bytesStored;
    *bytesStored = 0;

    do {
      if(bytesToStore < 1) {
        return buffer;
      }

      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      if(codepoint == 0) {
        return buffer;
      }

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength, encoding);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL ? ((char)codepoint) : bytes[i], &buffer, bytesStored);
      }

      bytesToStore -= bytesMatched;
      s += bytesMatched;
    } while(1);
  }

  /* get until a null byte in the source. a trailing null byte isn't added */
  if(query == NULL || !(query->params & (PRM_INSERT | PRM_REMOVE))) {
    do {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      if(codepoint == 0) {
        return buffer;
      }

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength, encoding);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
      }

      s += bytesMatched;
    } while(1);
  }

  do {
    if(query->params & PRM_INSERT) {
      if(query->codepointsInLine == 65) {
        codepoint = getUnicodeCharFast((unsigned char *)((query->newLine)+1), &bytesMatched);
        bytesMatched = 0;

        query->codepointsInLine = 0;
      }
      else if(query->codepointsInLine == 64) {
        codepoint = getUnicodeCharFast((unsigned char *)(query->newLine), &bytesMatched);
        bytesMatched = 0;

        if((query->newLine)[1] == 0) {
          query->codepointsInLine = 0;
        }
        else {
          /* newLine can only be \r\n in this case */
          query->codepointsInLine += 1;
        }
      }
      else {
        codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

        if(codepoint == 0) {
          return buffer;
        }

        if(codepoint == 0x0A || codepoint == 0x0D) {
          query->codepointsInLine = 0;
        }
        else {
          query->codepointsInLine += 1;
        }
      }
    }
    else /*if(query->params & PRM_REMOVE) */ {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      if(codepoint == 0) {
        return buffer;
      }

      if(query->codepointsInLine == 65) {
        if(codepoint == 0x0A) {
          query->codepointsInLine = 0;
          s += bytesMatched;
          continue;
        }

        query->codepointsInLine = -1;
      }
      else if(query->codepointsInLine == 64) {
        /* skip newline characters on the 64 codepoint boundary */
        if(codepoint == 0x0D) {
          query->codepointsInLine += 1;
          s += bytesMatched;
          continue;
        }

        if(codepoint == 0x0A) {
          query->codepointsInLine = 0;
          s += bytesMatched;
          continue;
        }

        query->codepointsInLine = -1;
      }

      if(codepoint == 0x0D || codepoint == 0x0A) {
        query->codepointsInLine = 0;
      }
      else {
        query->codepointsInLine += 1;
      }
    }

    /* The encoding is only ever utf-8 with d_charsetEncode if the
     * PRM_INSERT OR PRM_REMOVE is enabled. in these cases don't bother
     * calling the getBytes function, just call strAppendUTF8 instead */
    if(encoding == ENC_UTF8) {
      strAppendUTF8(codepoint, (unsigned char **)(&buffer), bytesStored);
    }
    else {
      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength, encoding);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
      }
    }

    s += bytesMatched;
  } while(1);
}
