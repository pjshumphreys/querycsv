void strSlice(char ** output, char * input, long beginIndex, long endIndex) {
  char * temp = NULL;
  long currentIndex = 0;
  size_t strSize = 0;

  if(output == NULL || input == NULL) {
    fputs(TDB_INVALID_STRSLICE, stderr);
    exit(EXIT_FAILURE);
  }

  /* If either beginIndex or EndIndex is negative we need to get the number of codepoints in the original string first */
  if(beginIndex < 0 || endIndex < 1) {
    /* count the length of the original string in terms of unicode code points */
    temp = input;

    while(*temp != 0) {
      /* count ASCII chars and UTF-8 lead bytes. We assume the utf-8 is valid at this point */
      if(((unsigned char)(*temp)) < 0x80 || ((unsigned char)(*temp)) > 0xBF) {
        currentIndex++;
      }

      temp++;
    }

    /* update beginIndex and endIndex with our results */
    if(beginIndex < 0) {
      beginIndex += currentIndex;
    }

    if(endIndex < 1) {
      endIndex += currentIndex;
    }
  }

  temp = NULL;

  /* if beginIndex >= endIndex, we can quickly just return an empty string */
  if(beginIndex < endIndex) {
    currentIndex = 0;

    /* loop over the original string again, and add characters to the output string if they fall within the specified range */
    while(*input != 0) {
      /* count ASCII chars and UTF-8 lead bytes. We assume the utf-8 is valid at this point */
      if(((unsigned char)(*input)) < 0x80 || ((unsigned char)(*input)) > 0xBF) {
        currentIndex++;
      }

      if(currentIndex > endIndex) {
        break;
      }

      if(currentIndex > beginIndex) {
        strAppend(*input, &temp, &strSize);
      }

      input++;
    }
  }

  /* null terminate the string */
  strAppend('\0', &temp, &strSize);

  /* return the new string we've created */
  free(*output);
  *output = temp;
}
