#include "querycsv.h"
#undef main
int realmain(int argc, char **argv);

#if defined(__VBCC__) || defined(__TURBOC__) || defined(LATTICE)
  #ifdef __VBCC__
    #include <gem.h>
  #else
    #include <vdi.h>
    #include <aes.h>
  #endif

  #include <tos.h>
#else
  #include <obdefs.h>
  #include <define.h>
  #include <gemdefs.h>
  #include <osbind.h>
#endif

void onShutdown(void) {
  int x;

  fputs(TDB_PRESS_A_KEY, stdout);

  x = Crawcin();
}

int main(int argc, char** argv) {
  devNull = "NUL";  /* null filename on Atari TOS */

  atexit(onShutdown);

  return realmain(argc, argv);
}
