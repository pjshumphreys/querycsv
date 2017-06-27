#include "querycsv.h"

int fputsEncoded(char * str, FILE * stream, int encoding) {
  size_t bytesStored;
  char *encoded = NULL;
  int retval;

  switch(encoding) {
    case ENC_UTF16LE:
    case ENC_UTF16BE:
    case ENC_UTF32LE:
    case ENC_UTF32BE:
      if(stream != stdout && stream != stderr) {
        break;
      }
    case ENC_UTF8:
      return fputs(str, stream);
  }

  bytesStored = 0;
  encoded = d_charsetEncode(str, encoding, &bytesStored);
  retval = fwrite(encoded, sizeof(char), bytesStored, stream);

  free(encoded);

  return retval;
}
