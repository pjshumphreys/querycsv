int mystrnicmp(const char *str1, const char *str2, size_t n) {
  int i, retval = 0;

  if(n) {
    for(i = 0; i < n; ++i) {
      retval = tolower(*str1++) - tolower(*str2++);

      if(retval) {
        break;
      }

      if(*str1 && *str2) {
        continue;
      }
      else {
        break;
      }
    }
  }

  return retval;
}
