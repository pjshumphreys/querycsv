#include "querycsv.h"

void stringGet(
    unsigned char **str,
    struct resultColumnValue *field,
    int params
) {
  MAC_YIELD

  if(str != NULL) {
    if(*str != NULL) {
      free(*str);
    }

    *str = (unsigned char *)(mystrdup((char const *)(field->value)));
  }
}
