#include "querycsv.h"

/*skips the BOM if present from a file */
FILE *skipBom(const char *filename, long* offset) {
  FILE *file;

  MAC_YIELD

  file = fopen(filename, "rb");

  if (file != NULL) {
    /* skip over the bom if present */
    if(fgetc(file) == 239 && fgetc(file) == 187 && fgetc(file) == 191) {
      if(offset) {
        *offset = 3;
      }

      return file;
    }

    /* the byte order mark was not found, and calling ungetc multiple times is not */
    /* portable (doesn't work on cc65). Therefore we just close and reopen the file */
    fclose(file);

    file = fopen(filename, "rb");
  }

  if(offset) {
    *offset = 0;
  }

  return file;
}
