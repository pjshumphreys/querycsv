#if !defined(__Z88DK)
int mystrnicmp(const char *str1, const char *str2, size_t n) {
  int i, retval = 0;

  unsigned char a, b;

  MAC_YIELD

  if(n) {
    for(i = 0; i < n; ++i) {
      a = *str1++;
      b = *str2++;

      if(a >= 'A' && a <= 'Z') {
        a = a - 'A' + 'a';
      }

      if(b >= 'A' && b <= 'Z') {
        b = b - 'A' + 'a';
      }

      retval = a - b;

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
#endif
