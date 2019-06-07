/*  d_strtok - tokenise a string without modifying it using dynamic memory allocation

tokenise a string similarly to the c standard lib strtok, but the
original string pointer is now the third parameter, the string data is
not modifed but instead that pointer is incremented, and the results are
allocated/ freed internally by realloc. A pointer to the original string
must be kept outside this function separate to the one passed as the third
parameter */
void d_strtok(char** result, char* delimiters, char** startFrom) {
  char * delimiterTest = delimiters;
  char * retval = NULL;
  size_t strSize = 0;
  char * startLocation;
  char * location;

  if(result == NULL) {
    return;
  }

  freeAndZero(*result);

  if(
    delimiters == NULL ||
    startFrom == NULL ||
    *startFrom == NULL
  ) {
    return;
  }

  startLocation = *startFrom;

  /* find the first non matching character */
  do {
    if(*startLocation == 0) {
      /* no match found. just quit */
      return;
    }

    if(*delimiterTest == 0) {
      /* no delimiters were found. we can start looking for a match */
      break;
    }

    if(*startLocation == *delimiterTest) {
      /* a match was found. try the next character */
      startLocation++;
      delimiterTest = delimiters;
    }
    else {
      delimiterTest++;
    }

  } while(1);

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
    }
    else {
      delimiterTest++;
    }
  }

  *startFrom = location;

  reallocMsg((void **)&retval, strSize+1);

  strncpy(retval, startLocation, strSize);

  /* NULL terminate the return value */
  retval[strSize] = '\0';

  *result = retval;
}
