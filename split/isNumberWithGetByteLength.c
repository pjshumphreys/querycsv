#include "querycsv.h"

int isNumberWithGetByteLength(
    unsigned char *offset,
    int *lastMatchedBytes,
    int firstChar
) {
  int decimalNotFound = TRUE;
  unsigned char *string = offset;

  MAC_YIELD

  if(
      (*offset >= '0' && *offset <= '9') ||
      (firstChar && (
      (*offset == '.' && (*(offset+1) >= '0' && *(offset+1) <= '9')) ||
      (*offset == '-' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9')))) ||
      (*offset == '+' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9'))))
      ))) {

    if(*string == '-' || *string == '+') {
      string++;
    }

    while(
      (*string >= '0' && *string <= '9') ||
      (decimalNotFound && (*string == '.' || *string == ',') &&
      !(decimalNotFound = FALSE))
    ) {
      string++;
    }

    *(lastMatchedBytes)+=(string-offset)-1;

    return TRUE;
  }
  else {
    return FALSE;
  }
}
