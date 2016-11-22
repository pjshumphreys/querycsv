#include "querycsv.h"

/* fake only the skipping of bytes and presume the file is already rewound */
int myfseek(FILE *stream, long offset, int origin) {
  long current;
  int c;  /* discarded */

  for(current = 0; current<offset; current++) {
    c = fgetc(stream);
  }

  return 0;
}
