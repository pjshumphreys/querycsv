#include "cc65iso.h"
#include "en_gb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const unsigned short petscii[128];
extern char spinnerEnabled;
extern char cursorOutput;
extern char currentWaitCursor;
extern char * spinner;
extern char * currentWaitCursorChar;

char* mystrdup(const char* s);
int strAppendUTF8(long codepoint, unsigned char **nfdString, size_t *retval);
int d_sprintf(char **str, char *format, ...);
char *d_charsetEncode(char* s, int encoding, size_t *bytesStored, struct qryData *query);

#define ENC_PETSCII 13
#define TRUE 1
#define FALSE 0

int stricmp_c64(const char *str1, const char *str2) {
  int retval = 0;

  unsigned char a, b;

  for(;;) {
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

  return retval;
}


/*
  updateSpinner - This function always needs to be mapped in, so we've
  precompiled it and will prepend its output assembly language code (modified
  to inline any runtime functions) to yyparse's oversize page
*/
/*void updateSpinner(void) {
  if(spinnerEnabled) {
    if(cursorOutput) {
      __asm__ ("lda #8");
      __asm__ ("jsr $ffd2");
    }

    do {
      currentWaitCursor = *currentWaitCursorChar;

      if(currentWaitCursor) {
        break;
      }

      currentWaitCursorChar = spinner;
    } while(1);

    /* spinner[currentWaitCursor] will still be in register a here * /
    __asm__ ("jsr $ffd2");

    currentWaitCursorChar+=1;
    cursorOutput = 1;
  }
}
*/

int fputs_c64(const char *str, FILE *stream) {
  size_t bytesStored;
  char *encoded = NULL;

  if(stream == stdout || stream == stderr) {
    bytesStored = 0;

    /* toggleSpinner(0); inlined */
    spinnerEnabled = FALSE;

    if(cursorOutput != FALSE) {
      __asm__ ("lda #$9d");
      __asm__ ("jsr $ffd2");
      cursorOutput = FALSE;
    }

    encoded = d_charsetEncode((char *)str, ENC_PETSCII, &bytesStored, NULL);
    fwrite(encoded, sizeof(char), bytesStored, stream);
    free(encoded);

    return (int)bytesStored;
  }
  else {
    return fwrite(str, sizeof(char), strlen(str), stream);
  }
}

int fprintf_c64(FILE *stream, const char *format, ...) {
  va_list args;
  size_t newSize;
  char* newStr = NULL;

  if(format == NULL) {
    return FALSE;
  }

  //get the space needed for the new string
  va_start(args, format);
  newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
  va_end(args);

  //Create a new block of memory with the correct size rather than using realloc
  //as any old values could overlap with the format string. quit on failure
  if((newStr = (char*)malloc(newSize+1)) == NULL) {
    return FALSE;
  }

  //do the string formatting for real
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = '\0';

  fputs_c64(newStr, stream);

  free(newStr);

  return newSize-1;
}

FILE *fopen_c64(const char *filename, const char *mode) {
  FILE * retval;
  char * encoded = d_charsetEncode((char *)filename, ENC_PETSCII, NULL, NULL);

  retval = fopen(encoded, mode);

  free(encoded);

  if(retval != NULL && strstr(mode, "R")) {
    /* skip petscii load address. All files loaded by the c64 build are assumed
     * to have this. If a file doesn't, add 2 spaces to the beginning of it and they'll be ignored in most cases */
    fgetc(retval);
    fgetc(retval);
  }

  return retval;
}

char * petsciiToUtf8(char *input) {
  char * encoded = NULL;
  unsigned char * temp = input;
  size_t tempSize = 0;
  long c;

  if(*temp) {
    while(*temp) {
      if(*temp < 65) {
        c = (long)*temp;
      }
      else if(*temp > 191) {
        c = (long)(0xFFFD);
      }
      else {
        c = (long)(petscii[(*temp)-64]);
      }

      strAppendUTF8(c, &encoded, &tempSize);

      temp++;
    }

    strAppendUTF8(0, &encoded, &tempSize);

    return encoded;
  }

  return mystrdup("");
}
