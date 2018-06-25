#include <ctype.h>

int stricmp(const char *str1, const char *str2) {
  const unsigned char *p1 = (unsigned char *)str1-1;
  const unsigned char *p2 = (unsigned char *)str2-1;
  unsigned long c1, c2;

  while(tolower(c1 = *++p1) == tolower(c2 = *++p2)) {
    if(!c1) {
      return 0;
    }
  }

  return c1 - c2;
}
