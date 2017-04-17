#include "querycsv.h"

void stringGet(
    unsigned char **str,
    struct resultColumnValue *field,
    int params
  ) {

  MAC_YIELD

  if(str != NULL) {
    *str = (unsigned char *)(strdup((char const *)(field->value)));
  }
}
