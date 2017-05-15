#include "querycsv.h"

char *charsetEncode_d(char* s, int encoding, size_t *bytesStored) {
  char * buffer = NULL;
  char c;
  long codepoint;
  int i;
  int byteLength;
  int bytesMatched;
  int didAllocateBytes = FALSE;
  char* bytes;
  void (*getBytes)(long, char **, int *);

  switch(encoding) {
    case ENC_CP1252: {
      getBytes = getBytesCP1252;
    } break;

    case ENC_CP437: {
      getBytes = getBytesCP437;
    } break;

    case ENC_CP850: {
      getBytes = getBytesCP850;
    } break;

    case ENC_MAC: {
      getBytes = getBytesMac;
    } break;

    case ENC_PETSCII: {
      getBytes = getBytesPetscii;
    } break;

    case ENC_CP1047: {
      getBytes = getBytesCP1047;
    } break;

    case ENC_UTF16LE: {
      getBytes = getBytesUtf16Le;
      reallocMsg(&bytes, 4);
      didAllocateBytes = TRUE;
    } break;

    default: {
      getBytes = getBytesCP1252;
    } break;
  }

  if(bytesStored != NULL) {
    for( ; ; ) {
      //call getUnicodeCharFast
      codepoint = getUnicodeCharFast(s, &bytesMatched);

      if(codepoint == 0) {
        if(didAllocateBytes) {
          free(bytes);
        }

        return buffer;
      }

      //get the bytes for the codepoint in the specified encoding (may be more than 1 byte)
      getBytes(codepoint, &bytes, &byteLength);

      //for each byte returned, call strAppend
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
      }

      s += bytesMatched;
    }
  }
  else {
    for( ; ; ) {
      //call getUnicodeCharFast
      codepoint = getUnicodeCharFast(s, &bytesMatched);

      //get the bytes for the codepoint in the specified encoding (may be more than 1 byte)
      getBytes(codepoint, &bytes, &byteLength);

      //for each byte returned, call strAppend
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
      }

      if(codepoint == 0) {
        if(didAllocateBytes) {
          free(bytes);
        }

        return buffer;
      }

      s += bytesMatched;
    }
  }
}

FILE *fopenEncoded(const char * filename, const char * mode, int encoding) {
  char *encoded = charsetEncode_d(filename, encoding, NULL);
  FILE *retval = fopen(encoded, mode);

  free(encoded);

  return retval;
}

int fputsEncoded(const char * str, FILE * stream, int encoding) {
  size_t bytesStored;
  char *encoded;
  int retval;

  if(encoding != ENC_UTF8) {
    bytesStored = 0;
    encoded = charsetEncode_d(str, encoding, &bytesStored);
    retval = fwrite(encoded, sizeof(char), bytesStored, stream);

    free(encoded);

    return retval;
  }
  
  return fputs(str, stream);
}
