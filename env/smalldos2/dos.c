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

#define ENC_UNKNOWN 0
#define ENC_CP437 1
#define ENC_CP850 2
#define ENC_CP1252 3
#define ENC_ASCII 7

extern char * devNull;
extern int origDrive;
extern char * origWd;
int consoleEncoding = ENC_UNKNOWN;
void openDat(void);
void atexit_dos(void);

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

  #ifdef DOS_DAT
    /* open the hash2 data file on startup */
    openDat();
  #endif

  /* set the working directory back to its original value at exit */
  atexit(atexit_dos);
}
