#include "querycsv.h"

char* __fastcall__ mystrdup(const char* s) {
  char *res = NULL;

  reallocMsg((void**)&res, strlen(s) + 1);

  strcpy(res, s);

  return res;
}
