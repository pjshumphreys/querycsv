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

#define ENC_UNKNOWN 0
#define ENC_CP437 1
#define ENC_CP850 2
#define ENC_CP1252 3
#define ENC_ASCII 7

#define freeAndZero(p) { free(p); p = 0; }

extern short int pageNumber;
extern short int success;

char pageName[13] = { 0 };
int handle;
unsigned int success2;

extern int origDrive;
extern char *origWd;
extern char *devNull;

short int lastWasErr = FALSE;
short int newline = FALSE;
short int currentWaitCursor = 0;
short int startOfLine = TRUE;
short int cursorOutput = FALSE;

extern int consoleEncoding;

extern FILE* mystdin;
extern FILE* mystdout;
extern FILE* mystderr;

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
  if(pageNumber) {
    sprintf(pageName, "qcsv%02dpc.ovl", pageNumber);

    if(_dos_open(pageName, O_RDONLY, &handle) != 0) {
      fprintf(stderr, "Couldn't find %s", pageName);
      fflush(stderr);
      success = 0;
      return;
    }

    success = _dos_read(handle, buffer, 16384, &success2);
    _dos_close(handle);

    if(success != 0 || success2 != 16384) {
      fprintf(stderr, "Couldn't read from %s", pageName);
      fflush(stderr);
      success = 0;
      return;
    }
  }

  success = 1;
}

void macYield(int num) {
  const char * spinner = "...ooOOoo";
  FILE * pFile = fopen("log.txt", "a");

  if(pFile != NULL) {
    fprintf(pFile, "%05d\n", num);
    fclose (pFile);
  }

  if(startOfLine) {
    if(cursorOutput) {
      fputc('\b', stderr);
    }

    fputc(spinner[currentWaitCursor], stderr);
    fflush(stderr);
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

  if(stream == mystdout || stream == mystderr) {
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

void setupDos(void) {
  union REGS regs;

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

  /* MSDOS needs the TZ environment variable set then
  setlocale to be called to properly calculate gmtime */

  /* supply some default timezone data if none is present */
  if(getenv("TZ") == NULL) {
    putenv(TDB_DEFAULT_TZ);
  }

  /* update the timezone info from the tz environmant variable */
  tzset();

  /* get the original drive and working directory to be able */
  /* to revert them if they need to be changed during runtime */
  origDrive = _getdrive();
  origWd = getcwd(NULL, PATH_MAX + 1);

  /* set the working directory back to its original value at exit */
  atexit(atexit_dos);

  mystdin = stdin;
  mystdout = stdout;
  mystderr = stderr;
}

void b(void) {
  static char * string;
  static double d, e;

  static FILE* test;
  static int num;
  static unsigned long num2;
  static long num3;
  union REGS regs;
  time_t now;

  num3 = atol(string);
  num3 = strcmp(string, string);
  sprintf(string, "%g %ld", d, num3);

  abs(num);
  strcpy(string, string);
  strncpy(string, string, 3);
  num = stricmp(string, string);
  num = strnicmp(string, string, 3);
  string = strstr(string, string);
  string = strrchr(string, ',');
  vsnprintf(NULL, 0, "%s%ld%d", NULL);

  memset(string, 0, 4);
  strcat(string, string);
  strncat(string, string, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);
  getcwd(string, PATH_MAX);
  fwrite(string, 1, 1, test);

  fseek(test, 9, SEEK_SET);
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fflush(stdout);
  num = isspace(num);
  num = isdigit(num);
  atexit(atexit_dos);
  time(&now);
  localtime(&now);
  gmtime(&now);
  atexit(dosload);
  bsearch(NULL,NULL, 3,3, NULL);
  qsort(NULL, 3, 3, NULL);
  chdir(NULL);
  _chdrive(0);
}
