#include "querycsv.h"

FILE *skipBom(const char *filename)
/*skips the BOM if present from a file */
{
  FILE * file;
  int c, c2, c3;

  file = fopen(filename, "rb");

  if (file != NULL) {
    if((c = fgetc(file)) == 239) {
      if((c2 = fgetc(file)) == 187) {
        if((c3 = fgetc(file)) == 191) {
          return file;
        }

        ungetc(c3, file);
      }

      ungetc(c2, file);
    }

    ungetc(c, file);
  }

  return file;
}
