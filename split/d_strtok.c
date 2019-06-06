void d_strtok(char** result, unsigned char* delimiters, char** startFrom) {
  char * startLocation = NULL;
  char * location = NULL;
  char * delimiterTest;
  char * retval = NULL;
  size_t strSize = 0;

  if(result == NULL) {
    return;
  }

  freeAndZero(*result);

  if(
    delimiters == NULL ||
    startFrom == NULL ||
    *startFrom == NULL ||
    **startFrom == '\0'
  ) {
    return;
  }

  startLocation = *startFrom;
  delimiterTest = delimiters;

  /* find the first non matching character */
  while(*startLocation) {
    if(*startLocation == *delimiterTest) {
      /* a match was found. try the next character */
      startLocation++;
      delimiterTest = delimiters;
      continue;
    }

    if(*delimiterTest == 0) {
      /* no delimiters matched. we can start looking for a match */
      delimiterTest = delimiters;
      break;
    }

    /* no delimiters have been found yet. keep trying */
    delimiterTest++;
  }

  location = startLocation;

  /* keep increasing the count of characters while no delimiters get matched */
  while(*location) {
    if(*location == *delimiterTest) {
      /* a delimiter was matched. break out of the loop */
      break;
    }

    if(*delimiterTest == 0) {
      /* No delimiters matched. increase the size */
      location++;
      strSize++;
      delimiterTest = delimiters;

      continue;
    }

    delimiterTest++;
  }

  if(strSize) {
    reallocMsg((void **)&retval, strSize);

    strncpy(retval, startLocation, strSize);

    *startFrom = location;

    /* NULL terminate the return value */
    strAppend('\0', &retval, &strSize);

    *result = retval;
  }
}