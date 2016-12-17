#include "floatlib/float.h"

#define NULL ((void *)0)

void _strtod(void);

#pragma codeseg ("FLOATLIB")

double strtod(unsigned char* text, unsigned char** bytesMatched) {
  unsigned char y = 0; 
  unsigned char y2;
   
  for(;;) {
    y2 = text[y];
    if(y2 <= '9' && y2 >= '0') {
test:
      ++y;
      continue;
    }
    else switch(y2) {
      case ' ':
      case '-':
      case '+':
      case 'E':
      case '.':
        goto test;
      break;
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
  if(bytesMatched != NULL) {
    *bytesMatched = text;
  }

  return ctof(0);
}

#pragma rodataseg ("FAKERODATA")
#pragma dataseg ("FAKEDATA")
#pragma bssseg ("FAKEDATA")
#pragma codeseg ("FOO")

int main(int argc, char **argv) {
  return 0;
}
