/* fake program to get the necessary libc functions into 1 memory page */
extern char buffer[16384];

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>  /* we'll be using the int86 function in dos.h to get the system codepage */

#ifdef __TURBOC__
  #include <dir.h>
  #define _chdrive setdisk
#else
  #include <direct.h>
#endif

#include <time.h>
#include <fcntl.h>
#include <io.h>

#include "en_gb.h"

#define FALSE 0
#define TRUE  1

#define freeAndZero(p) { free(p); p = 0; }

unsigned int success = 0;
char pageName[] = "qrycsv01.ovl";
int handle = 0;
int origDrive;
char *origWd;
char *devNull;
static short int lastWasErr = FALSE;
static short int newline = FALSE;
static short int currentWaitCursor = 0;
static short int startOfLine = TRUE;
static short int cursorOutput = FALSE;
extern short int pageNumber;

#ifdef DOS_DAT
  extern FILE* datafile;
#endif

void atexit_dos(void) {
  _chdrive(origDrive);

  chdir(origWd);
  freeAndZero(origWd);

  #ifdef DOS_DAT
    if(datafile != NULL) {
      fclose(datafile);
    }
  #endif
}

void dosload(void) {
  if(_dos_open(pageName, O_RDONLY, &handle) != 0) {
    success = 0;
    return;
  }

  _dos_read(handle, buffer, 16384, &success);
  _dos_close(handle);
  success = 1;
}

void macYield(void) {
  const char * spinner = "...ooOOoo";
  if(startOfLine) {
    if(cursorOutput) {
      fputc('\b', stderr);
    }

    fputc(spinner[currentWaitCursor], stderr);
    currentWaitCursor = (currentWaitCursor + 1) % 9;

    cursorOutput = TRUE;
  }
}

/* eat the last newline emitted as dos will add one back */
size_t fwrite_dos(const void * ptr, size_t size, size_t count, FILE * stream) {
  size_t len = size * count;
  size_t written = len;

  if(stream != stdout && stream != stderr) {
    return fwrite(ptr, size, count, stream);
  }

  startOfLine = FALSE;

  if(cursorOutput) {
    fputc('\b', stderr);
    cursorOutput = FALSE;
  }

  if(newline) {
    fputc('\n', lastWasErr ? stderr : stdout);

    newline = FALSE;
    written++;
  }

  /* eat last trailing newline (if we print something else we'll display it then) */
  if(((char*)ptr)[len-1] == '\n') {
    newline = TRUE;
    lastWasErr = stream == stderr;
    len--;
    written--;
  }

  fwrite(ptr, 1, len, stream);

  return written;
}

int fputs_dos(const char *str, FILE *stream) {
  return fwrite_dos(str, 1, strlen(str), stream);
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
    if((newStr = (char*)malloc(newSize)) == NULL) {
      return FALSE;
    }

    //do the string formatting for real
    va_start(args, format);
    vsprintf(newStr, format, args);
    va_end(args);

    fwrite_dos(newStr, 1, newSize, stream);

    free(newStr);

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  return retval;
}

int main2(int argc, char** argv);

int main(int argc, char** argv) {
  return main2(argc, argv);
}

void b(void) {
  static char * string;
  static double d;

  static FILE* test;
  static int num;
  static unsigned long num2;
  static long num3;
  union REGS regs;
  time_t now;

  num = atol(string);
  sprintf(string, "%g %ld", d, num3);

  abs(num);
  strncpy(string, string, 3);
  num = strcmp(string, string);
  num = stricmp(string, string);
  num = strnicmp(string, string, 3);
  string = strstr(string, string);
  strrchr(string, ',');
  vsnprintf(NULL, 0, "%s%ld%d", NULL);

  memset(string, 0, 4);
  strcat(string, string);
  strncat(string, string, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);
  fwrite(string, 1, 1, test);

  fseek(test, 9, SEEK_SET);
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fflush(stdout);
  isspace(num);
  isdigit(num);
  atexit(atexit_dos);
  putenv(getenv("TZ"));
  int86(0x21, &regs, &regs);
  tzset();
  origDrive = _getdrive();
  origWd = getcwd(NULL, PATH_MAX + 1);
  localtime(&now);
  gmtime(&now);
}
