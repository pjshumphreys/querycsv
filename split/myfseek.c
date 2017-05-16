#include "querycsv.h"

/* fake only the skipping of bytes and presume the file is already rewound */
int myfseek(FILE *stream, long offset, int origin) {
  long current;

  for(current = 0; current<offset; current++) {
    fgetc(stream);
  }

  return 0;
}
