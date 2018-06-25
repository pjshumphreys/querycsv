char *d_charsetEncode(char* s, int encoding, size_t *bytesStored) {
  char * buffer = NULL;
  long codepoint;
  int i;
  int byteLength;
  int bytesMatched;
  int bytesToStore;
  char* bytes = NULL; /* a pointer to constant data *OR* some allocated data for utf-16 */
  char utf16Bytes[4];
  void (*getBytes)(long, char **, int *);
  size_t temp;

  MAC_YIELD

  switch(encoding) {
    case ENC_CP1252: {
      getBytes = getBytesCP1252;
    } break;

    case ENC_CP437: {
      getBytes = getBytesCP437;
    } break;

    case ENC_CP850: {
      getBytes = getBytesCP850;
    } break;

    case ENC_MAC: {
      getBytes = getBytesMac;
    } break;

    case ENC_PETSCII: {
      getBytes = getBytesPetscii;
    } break;

    case ENC_CP1047: {
      getBytes = getBytesCP1047;
    } break;

    case ENC_ATARIST: {
      getBytes = getBytesAtariST;
    } break;

    case ENC_UTF16LE: {
      getBytes = getBytesUtf16Le;
      bytes = (char *)utf16Bytes;
    } break;

    case ENC_UTF16BE: {
      getBytes = getBytesUtf16Be;
      bytes = (char *)utf16Bytes;
    } break;

    default: {
      getBytes = getBytesCP1252;
    } break;
  }

  /* if we don't care how long the returned string is. always includes trailing null byte */
  if(bytesStored == NULL) {
    temp = 0;

    for( ; ; ) {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL ? ((char)codepoint) : bytes[i], &buffer, &temp);
      }

      if(codepoint == 0) {
        return buffer;
      }

      s += bytesMatched;
    }
  }

  /* if bytesStored is initially non zero, it means limit to read at most that many characters from the source. a trailing null byte isn't added */
  if(*bytesStored != 0) {
    bytesToStore = (int)*bytesStored;
    *bytesStored = 0;

    for( ; ; ) {
      if(bytesToStore < 1) {
        return buffer;
      }

      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      if(codepoint == 0) {
        return buffer;
      }

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL ? ((char)codepoint) : bytes[i], &buffer, bytesStored);
      }

      bytesToStore -= bytesMatched;
      s += bytesMatched;
    }
  }

  /* get until a null byte in the source. a trailing null byte isn't added */
  for( ; ; ) {
    /* call getUnicodeCharFast */
    codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

    if(codepoint == 0) {
      return buffer;
    }

    /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
    getBytes(codepoint, &bytes, &byteLength);

    /* for each byte returned, call strAppend */
    for(i=0; i < byteLength; i++) {
      strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
    }

    s += bytesMatched;
  }
}
