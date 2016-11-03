#include "querycsv.h"

#ifndef HAS_STRDUP

char* strdup(const char* s) {
  char *res = (char*)malloc(strlen(s) + 1);
  if (!res)
	return (char*)0;
  strcpy(res, s);
  return res;
}
#endif
