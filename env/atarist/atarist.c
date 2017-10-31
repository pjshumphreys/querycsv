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

int stricmp(const char *str1, const char *str2) {
  const unsigned char *p1 = (unsigned char *)str1-1;
  const unsigned char *p2 = (unsigned char *)str2-1;
  unsigned long c1, c2;

  while(tolower(c1 = *++p1) == tolower(c2 = *++p2)){
    if(!c1) {
      return(0);
    }
  }

  return(c1 - c2);
}

int main(int argc, char** argv) {

  return realmain(argc, argv);
}
