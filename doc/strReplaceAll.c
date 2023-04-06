char* strReplaceAll(char ** searches, char ** replacements, char * subject) {
  size_t length = 0;
  char** currentSearch = searches;
  char** currentReplacement = replacements;
  size_t currentLength;

  char* temp = subject;
  char* temp2;
  char* retval = NULL;

  if(!searches || !(*searches)) {
    return strdup(subject);
  }

  for(; *temp ;) {
    do {
      currentLength = strlen(*currentSearch);

      if(!strncmp(temp, *currentSearch, currentLength)) {
        temp += currentLength;

        currentLength = strlen(*currentReplacement);
        length += currentLength;
        break;
      }

      currentReplacement++;
      currentSearch++;
    } while(*currentSearch);

    if(!(*currentSearch)) {
      temp++;
      length++;
    }

    currentSearch = searches;
    currentReplacement = replacements;
  }

  retval = malloc(length+1);

  if(!retval) {
    return NULL;
  }

  temp = subject;
  temp2 = retval;
  currentSearch = searches;
  currentReplacement = replacements;

  for(;*temp;) {
    do {
      currentLength = strlen(*currentSearch);

      if(!strncmp(temp, *currentSearch, currentLength)) {
        temp += currentLength;

        currentLength = strlen(*currentReplacement);
        memcpy(temp2, *currentReplacement, currentLength);
        temp2 += currentLength;
        break;
      }

      currentReplacement++;
      currentSearch++;
    } while(*currentSearch);

    if(!(*currentSearch)) {
      *temp2 = *temp;
      temp++;
      temp2++;
    }

    currentSearch = searches;
    currentReplacement = replacements;
  }

  *temp2 = '\0';

  return retval;
}
