#include "querycsv.h"

#ifndef HAS_STRDUP

char* __fastcall__ strdup(const char* s) {
  char *res = NULL;

  reallocMsg((void**)&res, strlen(s) + 1);

  strcpy(res, s);
  return res;
}
#endif
