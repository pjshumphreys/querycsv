#include "querycsv.h"

/* append a character into a string with a given length, using realloc */
int strAppend(char c, char **value, size_t *strSize) {
  char *temp;

  MAC_YIELD

  /* validate inputs */
  /* increase value length by 1 character */

  /* update the string pointer */
  /* increment strSize */
  if(strSize != NULL) {
    if(value != NULL) {
      if((temp = realloc(*value, (*strSize)+1)) != NULL) {
      *value = temp;

      /* store the additional character */
      (*value)[*strSize] = c;
      }
      else {
      return FALSE;
      }
    }

    (*strSize)++;
  }

  return TRUE;
}
