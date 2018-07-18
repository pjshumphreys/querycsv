char *strReplace(char *search, char *replace, char *subject) {
  char *replaced, *temp = NULL;
  char *p = subject, *p3 = subject, *p2;
  int found = 0;

  MAC_YIELD

  if(
      search == NULL ||
      replace == NULL ||
      subject == NULL ||
      strlen(search) == 0 ||
      strlen(replace) == 0 ||
      strlen(subject) == 0
    ) {
    return mystrdup("");
  }

  replaced = (char*)calloc(1, 1);

  while((p = strstr(p, search)) != NULL) {
    found = 1;
    temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace) + 1);

    if(temp == NULL) {
      free(replaced);
      return mystrdup("");
    }

    replaced = temp;
    strncat(replaced, p - (p - p3), p - p3);
    strcat(replaced, replace);
    p3 = p + strlen(search);
    p += strlen(search);
    p2 = p;
  }

  if(found == 1) {
    if(strlen(p2) > 0) {
      temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);

      if(temp == NULL) {
        free(replaced);
        return mystrdup("");
      }

      replaced = temp;
      strcat(replaced, p2);
    }
  }
  else {
    temp = realloc(replaced, strlen(subject) + 1);

    if(temp != NULL) {
      replaced = temp;
      strcpy(replaced, subject);
    }
  }

  return replaced;
}
