#include "querycsv.h"

/* free memory and set the pointer to NULL. this function should hopefully get inlined */
void strFree(char **str) {
  MAC_YIELD

  free(*str);
  *str = NULL;
}
