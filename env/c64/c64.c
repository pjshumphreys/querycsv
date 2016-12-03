/*
#include "floatlib/float.h"

double strtod(const char* str, char** endptr) {
  return ctof(0);
}
*/

#include <stdio.h>

char* waterspout = "Hello, world";

int main(void) {
  fputs(waterspout, stdout);

  for (;;) {
    ++*(unsigned char*)0xd020;
  }

  return 0;
}

