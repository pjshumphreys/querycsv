#include "querycsv.h"

char *charsetEncode_d(char* s, int encoding, size_t *bytesStored) {
  char * buffer = NULL;
  long codepoint;
  int i;
  int byteLength;
  int bytesMatched;
  int didAllocateBytes = FALSE;
  char* bytes = NULL;
  void (*getBytes)(long, char **, int *);
  size_t temp;

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
      reallocMsg((void**)&bytes, 4);
      didAllocateBytes = TRUE;
    } break;

    default: {
      getBytes = getBytesCP1252;
    } break;
  }

  if(bytesStored != NULL) {
    for( ; ; ) {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      if(codepoint == 0) {
        if(didAllocateBytes) {
          free(bytes);
        }

        return buffer;
      }

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, bytesStored);
      }

      s += bytesMatched;
    }
  }
  else {
    temp = 0;

    for( ; ; ) {
      /* call getUnicodeCharFast */
      codepoint = getUnicodeCharFast((unsigned char *)s, &bytesMatched);

      /* get the bytes for the codepoint in the specified encoding (may be more than 1 byte) */
      getBytes(codepoint, &bytes, &byteLength);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength; i++) {
        strAppend(bytes == NULL?((char)codepoint):bytes[i], &buffer, &temp);
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

FILE *fopenEncoded(char * filename, char * mode, int encoding) {
  char *encoded = charsetEncode_d(filename, encoding, NULL);
  FILE *retval = fopen(encoded, mode);

  free(encoded);

  return retval;
}

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
  encoded = charsetEncode_d(str, encoding, &bytesStored);
  retval = fwrite(encoded, sizeof(char), bytesStored, stream);

  free(encoded);

  return retval;
}
