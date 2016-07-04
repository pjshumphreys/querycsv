#include "querycsv.h"

//checks whether the end of file is the next character in the stream.
//Used by getMatchingRecord
int endOfFile(FILE* stream) {
  int c;

  c = fgetc(stream);
  ungetc(c, stream);

  return c != EOF;
}
