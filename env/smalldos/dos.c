#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dos.h>  /* we'll be using the int86 function in dos.h to get the system codepage */
#ifdef __TURBOC__
  #include <dir.h>
  #define _chdrive setdisk
#else
  #include <direct.h>
#endif
#include <time.h>
#include "en_gb.h"

#define FALSE 0
#define TRUE  1

#define ENC_UNKNOWN 0
#define ENC_CP437 1
#define ENC_CP850 2
#define ENC_CP1252 3
#define ENC_ASCII 7

int consoleEncoding = ENC_UNKNOWN;
FILE* mystdin = NULL;
FILE* mystdout = NULL;
FILE* mystderr = NULL;

void openDat(void);
void atexit_dos(void);

