#ifndef __Z88DK
int mystrnicmp(const char *str1, const char *str2, size_t n) {
  int i = 0, retval = 0;

  unsigned char a, b;

  MAC_YIELD

  do {
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

    if(n) {
      ++i;

      if(i == n) {
        break;
      }
    }
  } while(1);

  return retval;
}
#endif
