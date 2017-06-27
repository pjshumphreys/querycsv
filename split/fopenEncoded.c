#include "querycsv.h"

FILE *fopenEncoded(char * filename, char * mode, int encoding) {
  char *encoded = d_charsetEncode(filename, encoding, NULL);
  FILE *retval = fopen(encoded, mode);

  free(encoded);

  return retval;
}
