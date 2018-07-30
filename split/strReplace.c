char *strReplace(char *search, char *replace, char *subject) {
  char *replaced = NULL;
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

  reallocMsg((void**)&replaced, 1);
  replaced[0] = '\0';

  while((p = strstr(p, search)) != NULL) {
    found = 1;

    reallocMsg((void**)&replaced, strlen(replaced) + (p - p3) + strlen(replace) + 1);

    strncat(replaced, p - (p - p3), p - p3);
    strcat(replaced, replace);
    p3 = p + strlen(search);
    p += strlen(search);
    p2 = p;
  }

  if(found == 1) {
    if(strlen(p2) > 0) {
      reallocMsg((void**)&replaced, strlen(replaced) + strlen(p2) + 1);

      strcat(replaced, p2);
    }
  }
  else {
    reallocMsg((void**)&replaced, strlen(subject) + 1);

    strcpy(replaced, subject);
  }

  return replaced;
}
