#include "querycsv.h"

/* skips the BOM if present from a file. */
/* Also attempts to detect the file encoding for
  non iso-646 character sets if the encoding parameter is not null */
FILE *skipBom(const char *filename, long* offset, int* encoding) {
  FILE *file;
  int c;

  MAC_YIELD

  file = fopen(filename, "rb");

  if (file != NULL) {
    /* skip over the bom if present */
    switch(fgetc(file)) {
      case 0: {  /* maybe UTF-32BE */
        if(
            fgetc(file) == 0 &&
            fgetc(file) == 254 &&
            fgetc(file) == 255
        ) {
          if(offset) {
            *offset = 4;
          }

          if(encoding) {
            *encoding = ENC_UTF32BE;
          }

          return file;
        }
      } break;

      case 239: { /* maybe utf-8 bom */
        if(
            fgetc(file) == 187 &&
            fgetc(file) == 191
        ) {
          if(offset) {
            *offset = 3;
          }

          if(encoding) {
            *encoding = ENC_UTF8;
          }

          return file;
        }
      } break;

      case 254: { /* maybe UTF-16BE */
        if(fgetc(file) == 255) {
          if(offset) {
            *offset = 2;
          }

          if(encoding) {
            *encoding = ENC_UTF16BE;
          }

          return file;
        }
      } break;

      case 255: { /* maybe UTF-16LE or UTF-32LE */
        if(fgetc(file) == 254) {
          if(
            fgetc(file) == 0 &&
            fgetc(file) == 0
          ) { /* UTF-32LE */
            if(offset) {
              *offset = 4;
            }

            if(encoding) {
              *encoding = ENC_UTF32LE;
            }
          }
          else {  /* UTF-16LE */
            if(offset) {
              *offset = 2;
            }

            if(encoding) {
              *encoding = ENC_UTF16LE;
            }
          }

          return file;
        }
      } break;

      case 133:
      case 197: { /* maybe the letters 'enc'(oding) (case insensitive) in EBCDIC */
        if(
            encoding &&
            ((c = fgetc(file)) == 149 || c == 213) && /* 'N' */
            ((c = fgetc(file)) == 131 || c == 195) /* 'C' */
        ) {
          *encoding = ENC_CP1047;
        }
      } break;

      default: {
        if(encoding) {
          *encoding = ENC_UNKNOWN;
        }
      }
    }

    if(offset) {
      *offset = 0;
    }

    /* the byte order mark was not found, and calling ungetc multiple times is not */
    /* portable (doesn't work on cc65). Therefore we just close and reopen the file */
    fclose(file);

    file = fopen(filename, "rb");
  }

  return file;
}
