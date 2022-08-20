int normaliseAndGet(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    int bytesRead,
    struct hash2Entry* entry
) {
  size_t offsetInt = *offset - *str;
  unsigned char * nfdString = NULL;
  size_t nfdLength = offsetInt;
  int i = 0, j;
  QCSV_LONG * codepointBuffer = NULL;
  int bufferLength;
  QCSV_LONG codepoint;

  MAC_YIELD

  /* if the allocation failed, print an error messge and exit */
  reallocMsg((void**)&nfdString, strlen((const char *)(*str))+1);

  /* copy the string matched up to now directly into the output string */
  memcpy(nfdString, (void*)(*str), offsetInt);

  /* if we've done the work of finding an entry then we should make use of it to populate the codepoint array initially */
  if(plusBytes != 0 || entry == NULL) {
    bufferLength = 0;
  }
  else {
    bufferLength = entry->length;
    reallocMsg((void**)&codepointBuffer, bufferLength*sizeof(QCSV_LONG));
    memcpy(codepointBuffer, (void*)(entry->codepoints), bufferLength*sizeof(QCSV_LONG));
    entry = NULL;
    *offset += bytesRead;
  }

  do {  /* the get a codepoint code will break out of this loop */
    /* while there are still unexamined codepoints in the buffer */
    while(i != bufferLength) {
      if(isCombiningChar(codepointBuffer[i]) == 0) {
        /* if i=0 then there were no combining characters at the head of the buffer */
        /* if i=1 then there is only 1 combining character at the head of the */
        /* buffer. therefore it doesn't need sorting */
        if(i > 1) {
          /* TODO: qsort the combining characters that preceed this codepoint */
        }

        /* output all the codepoints up to and including this one as utf-8 sequences */
        for(j = 0; j <= i; j++) {
          strAppendUTF8(codepointBuffer[j], &nfdString, &nfdLength);
        }

        bufferLength-=++i;
        memmove((void*)codepointBuffer, (void*)&codepointBuffer[i], bufferLength*sizeof(QCSV_LONG)); /* downward in memory i number of codepoints */
        i = 0;
      }
      else {
        i++;
      }
    }

    /* try getting a unicode code point */
    /* if it's invalid or overlong, read the bytes as CP-437. and read the first byte using hash 1 */
    /* otherwise check if it's decomposable. */
    /* if it's decomposable, put the entire sequence into the buffer then continue */
    /* otherwise just put the codepoint into the buffer then continue */
    plusBytes = 0;
    bytesRead = 0;

    if(**offset < 0x80) {
      /* if offset is 0 then we've reached the end of the string. quit the do loop */
      if(**offset == 0) {
        break;
      }

      bytesRead = 1;

      /* read 1 byte. no overlong checks needed as a 1 byte code can */
      /* never be overlong, and is never a combining character */
      reallocMsg((void**)&codepointBuffer, (1+bufferLength)*sizeof(QCSV_LONG));
      codepointBuffer[bufferLength++] = (QCSV_LONG)(*((*offset)++));

      continue;
    }

    /* ensure the current byte is the start of a valid utf-8 sequence */
    if(**offset > 0xC1) {
      if(**offset < 0xE0) {
        /* read 2 bytes */
        bytesRead = 2;
        codepoint = (QCSV_LONG)(**offset) << 6;
        (*offset)++;
        codepoint += **offset;
        (*offset)++;
        codepoint -= 0x3080;
      }
      else if(**offset < 0xF0) {
        /* read 3 bytes */
        bytesRead = 3;
        codepoint = ((QCSV_LONG)(**offset) << 12);
        (*offset)++;
        codepoint += ((QCSV_LONG)(**offset) << 6);
        (*offset)++;
        codepoint += **offset;
        (*offset)++;
        codepoint -= 0xE2080;
      }
      else if(**offset < 0xF5) {
        /* read 4 bytes */
        bytesRead = 4;
        codepoint = ((QCSV_LONG)(**offset) << 18);
        (*offset)++;
        codepoint += ((QCSV_LONG)(**offset) << 12);
        (*offset)++;
        codepoint +=  ((QCSV_LONG)(**offset) << 6);
        (*offset)++;
        codepoint += **offset;
        (*offset)++;
        codepoint -= 0x3C82080;
      }
    }

    if(bytesRead == 0) {
      /* as getCodepoints now pre converts invalid bytes, this should
        never happen now. If it does, show a message and quit */
      fputs(TDB_INVALID_UTF8, stderr);
      exit(EXIT_FAILURE);
    }
    else if((entry = isInHash2(codepoint)) == NULL) {
      /* the codepoint we found was not decomposable. just put it in the buffer */
      reallocMsg((void**)&codepointBuffer, (1+bufferLength)*sizeof(QCSV_LONG));
      codepointBuffer[bufferLength] = codepoint;
      bufferLength += 1;
    }
    else {
      /* a decomposable codepoint was found in hash 2. */

      /* put the whole byte sequence into the buffer */
      reallocMsg((void**)&codepointBuffer, (bufferLength+(entry->length))*sizeof(QCSV_LONG));
      memcpy(&(codepointBuffer[bufferLength]), (void*)(entry->codepoints), (entry->length)*sizeof(QCSV_LONG));
      bufferLength += entry->length;
      entry = NULL;
    }
  } while(1);

  if(i > 1) {
    /* TODO: qsort the combining characters that preceed this codepoint */
  }

  /* output the rest of the codepoints (which will all be combining characters) */
  for(j = 0; j < i; j++) {
    strAppendUTF8(codepointBuffer[j], &nfdString, &nfdLength);
  }

  /* append null to the string */
  strAppendUTF8(0, &nfdString, &nfdLength);

  /* free the codepoint buffer */
  free(codepointBuffer);

  /* swap out the string that will be searched from now on. free the old version */
  free(*str);
  *str = nfdString;
  (*offset) = (unsigned char *)((*str) + offsetInt);

  /* the whole string has been normalized to nfd form. */
  /* now use the fast version to get the next codepoint */
  return getUnicodeChar(offset, str, plusBytes, bytesMatched);
}
