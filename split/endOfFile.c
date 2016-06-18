#include "querycsv.h"

int endOfFile(FILE* stream)
//checks whether the end of file is the next character in the stream.
//Used by getMatchingRecord
{
  int c;

  c = fgetc(stream);
  ungetc(c, stream);

  return c != EOF;
}
