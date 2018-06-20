#include <errno.h>
#include <unixlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* argv2[3];

void setupRiscOS(int *argc, char ***argv) {
  char* lastDot = NULL;
  char* pseudoWD = NULL;
  size_t lastDotLength = 0;

  if((*argc) == 2) {
    lastDot = strrchr((*argv)[1], '.');
    argv2[0] = "querycsv";
    argv2[1] = (*argv)[1];
    argv2[2] = NULL;

    if(lastDot != NULL) {
      argv2[1] = lastDot + 1;

      lastDotLength = (int)(lastDot - (*argv)[1]);

      pseudoWD = malloc(lastDotLength + 1);

      strncpy(pseudoWD, (*argv)[1], lastDotLength);
      pseudoWD[lastDotLength] = '\0';

      chdir(pseudoWD);

      free(pseudoWD);
    }

    argv = (char ***)(&argv2);
  }
}
