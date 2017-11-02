#include "querycsv.h"

/* fake only the skipping of bytes and presume the file is already rewound */
int myfseek(FILE *stream, long offset, int origin) {
  if(origin != SEEK_SET) {
    return -1;
  }

  for( ; offset; offset--) {
    fgetc(stream);
  }

  return 0;
}
