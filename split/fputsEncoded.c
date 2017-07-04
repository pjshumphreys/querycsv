#include "querycsv.h"

int fputsEncoded(char * str, FILE * stream, int encoding) {
  size_t bytesStored;
  char *encoded = NULL;
  int retval;

  if(encoding == ENC_UTF8) {
    return fputs(str, stream);
  }

  bytesStored = 0;
  encoded = d_charsetEncode(str, encoding, &bytesStored);
  retval = fwrite(encoded, sizeof(char), bytesStored, stream);

  free(encoded);

  return retval;
}
