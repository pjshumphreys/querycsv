char* mystrdup(const char* s) {
  char *res = NULL;

  MAC_YIELD

  if(s != NULL) {
    reallocMsg((void**)&res, strlen(s) + 1);

    strcpy(res, s);
  }

  return res;
}
