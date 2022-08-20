void strSlice(char ** output, char * input, int beginIndex, int endIndex) {
  size_t strSize = 0;
  int currentIndex = 0;
  char * temp;

  MAC_YIELD

  if(output == NULL || input == NULL) {
    fputs(TDB_INVALID_STRSLICE, stderr);
    exit(EXIT_FAILURE);
  }

  /* If either beginIndex or endIndex is negative we need to get the number of codepoints in the original string first */
  if(beginIndex < 0 || endIndex < 0) {
    /* count the length of the original string in terms of unicode code points */
    temp = input;

    while(*temp != 0) {
      /* count ASCII chars and UTF-8 lead bytes. We assume the utf-8 is valid at this point */
      if(((unsigned char)(*temp)) < 0x80 || ((unsigned char)(*temp)) > 0xBF) {
        ++currentIndex;
      }

      ++temp;
    }

    /* update beginIndex and endIndex with our results */
    if(beginIndex < 0) {
      beginIndex += currentIndex;
    }

    if(endIndex < 0) {
      endIndex += currentIndex;
    }

    currentIndex = 0;
  }

  temp = NULL;

  if(endIndex == 0) while(*input != 0) {
    /* loop over the original string again, and add characters to the output string if they fall within the specified range */

    /* count ASCII chars and UTF-8 lead bytes. We assume the utf-8 is valid at this point */
    if(((unsigned char)(*input)) < 0x80 || ((unsigned char)(*input)) > 0xBF) {
      ++currentIndex;
    }

    if(currentIndex > beginIndex) {
      strAppend(*input, &temp, &strSize);
    }

    ++input;
  }
  else if(beginIndex < endIndex) while(*input != 0) {
    /* loop over the original string again, and add characters to the output string if they fall within the specified range */

    /* count ASCII chars and UTF-8 lead bytes. We assume the utf-8 is valid at this point */
    if(((unsigned char)(*input)) < 0x80 || ((unsigned char)(*input)) > 0xBF) {
      if(++currentIndex > endIndex) {
        break;
      }
    }

    if(currentIndex > beginIndex) {
      strAppend(*input, &temp, &strSize);
    }

    ++input;
  }

  /* null terminate the string */
  strAppend('\0', &temp, &strSize);

  /* return the new string we've created */
  free(*output);
  *output = temp;
}
