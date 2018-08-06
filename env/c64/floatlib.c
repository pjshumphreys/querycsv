#include "floatlib/float.h"

#define NULL ((void *)0)

void _strtod(void);

#pragma code-name ("FLOATLIB")
double strtod(const char* str, char** endptr) {
  unsigned char *y = (unsigned char *)str; 
  unsigned char y2;
   
  for(;;) {
    y2 = *y;

    if(
        (y2 <= '9' && y2 >= '0') ||
        y2 == '-' ||
        y2 == ' ' ||
        y2 == '+' ||
        y2 == 'E' ||
        y2 == '.'
    ) {
      ++y;
      continue;
    }
    
    break;
  }

  /*if some potentially valid characters were found then defer to the assembly language code */
  if(y != 0) {
    __asm__("ldy #%o", y);
    __asm__("lda (sp),y");
    __asm__("tay");
    __asm__("jmp %v", _strtod);
  }

  /* otherwise, return that no characters were matched and return 0 as a double */
  if(endptr != NULL) {
    *endptr = y;
  }

  return ctof(0);
}

#pragma rodata-name ("FAKERODATA")
#pragma data-name ("FAKEDATA")
#pragma bss-name ("FAKEDATA")
#pragma code-name ("FAKECODE")

int main(int argc, char **argv) {
  return 0;
}
