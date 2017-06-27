#include "querycsv.h"

FILE *fopenEncoded(char * filename, char * mode, int encoding) {
  char *encoded = charsetEncode_d(filename, encoding, NULL);
  FILE *retval = fopen(encoded, mode);

  free(encoded);

  return retval;
}
