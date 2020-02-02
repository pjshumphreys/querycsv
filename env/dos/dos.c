#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dos.h>  /* we'll be using the int86 function in dos.h to get the system codepage */
#include <direct.h>

#define FALSE 0
#define TRUE  1

#define freeAndZero(p) { free(p); p = 0; }

extern char * devNull;
extern char * origWd;

int strAppend(char c, char **value, size_t *strSize);

char *d_charsetEncode(char* s, int encoding, size_t *bytesStored);
#define ENC_UTF16LE 8
#define ENC_CP437 1
#define ENC_CP850 2
#define ENC_CP1252 3
#define ENC_ASCII 17
#define ENC_UNKNOWN 0

static int lastWasErr = FALSE;
static int newline = FALSE;
static int consoleEncoding = ENC_UNKNOWN;

void atexit_dos(void) {
  chdir(origWd);
  freeAndZero(origWd);
}

/* eat the last newline emitted as dos will add one back */
int fputs_dos(const char *str, FILE *stream) {
  union REGS regs;
  size_t len = 0;
  char* output = NULL;
  unsigned long retval;

  if(stream == stdout || stream == stderr) {
    if(consoleEncoding == ENC_UNKNOWN) {
      regs.x.ax = 0x6601;
      regs.x.bx = 0;

      int86(0x21, &regs, &regs);

      if(regs.x.cflag != 0) {
        regs.x.bx = 0;
      }

      switch(regs.x.bx) {
        case 437: {
          consoleEncoding = ENC_CP437;
        } break;

        case 850: {
          consoleEncoding = ENC_CP850;
        } break;

        case 1252: {
          consoleEncoding = ENC_CP1252;
        } break;

        default: {
          consoleEncoding = ENC_ASCII;
        } break;
      }
    }

    if(newline) {
      fputs("\n", lastWasErr ? stderr : stdout);

      newline = FALSE;
    }

    output = d_charsetEncode((char *)str, consoleEncoding, &len);

    /* eat last trailing newline (if we print something else we'll display it then) */
    if(output && output[len-1] == '\n') {
      newline = TRUE;
      lastWasErr = stream == stderr;
      output[len-1] = '\0';
    }
    else {
      /*d_charsetEncode (correctly) doesn't null terminate here so we do it ourselves */
      strAppend('\0', &output, &len);
    }

    retval = fputs(output, stream);
    free(output);
    return retval;
  }

  return fputs(str, stream);
}

int fprintf_dos(FILE *stream, const char *format, ...) {
  va_list args;
  int retval;
  size_t newSize;
  char* newStr = NULL;
  FILE * pFile;

  if(stream == stdout || stream == stderr) {
    if(format == NULL || (pFile = fopen(devNull, "wb")) == NULL) {
      return FALSE;
    }

    //get the space needed for the new string
    va_start(args, format);
    newSize = (size_t)(vfprintf(pFile, format, args)); //plus L'\0'
    va_end(args);

    //close the file. We don't need to look at the return code as we were writing to /dev/null
    fclose(pFile);

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

    fputs_dos(newStr, stream);

    free(newStr);

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  return retval;
}
