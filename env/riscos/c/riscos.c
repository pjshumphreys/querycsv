#include <os.h>
#include <swis.h>
#include <unixlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char * pseudoWD = NULL;

FILE *fopen_ros(const char *filename, const char *mode) {
  size_t length;
  _kernel_oserror *errnum;
  char *temp = NULL;
  FILE * retval;

  errnum = _swix(OS_FSControl, _INR(0,5) | _OUT(5), 37, filename, 0, 0, pseudoWD, 0, &length);

  if(errnum) {
    length = 257;
  }
  else {
    length = (-length) + 1;
  }

  temp = malloc(length);

  if(temp != NULL) {
    errnum = _swix(OS_FSControl, _INR(0,5), 37, filename, temp, 0, pseudoWD, length);

    retval = fopen(temp, mode);

    free(temp);
  }
  else {
    retval = fopen("", mode);
  }

  return retval;
}

void shutdownfoo(void) {
  free(pseudoWD);
}

void setupRiscOS(int *argc, char ***argv) {
  static char* argv2[3];
  char* lastDot = NULL;
  size_t lastDotLength = 0;

  if((*argc) == 2) {
    lastDot = strrchr((*argv)[1], '.');
    argv2[0] = "querycsv";
    argv2[1] = (*argv)[1];
    argv2[2] = NULL;


    if(lastDot != NULL) {
      lastDotLength = (int)(lastDot-(*argv)[1]);

      pseudoWD = malloc(lastDotLength+2);

      strncpy(pseudoWD, (*argv)[1], lastDotLength+1);
      pseudoWD[lastDotLength+1] = '\0';

      atexit(shutdownfoo);
    }

    argv = (char ***)(&argv2);
  }
}
