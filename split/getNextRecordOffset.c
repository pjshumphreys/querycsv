#include "querycsv.h"

int getNextRecordOffset(char *inputFileName, long offset) {
  FILE *inputFile = NULL;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  /* seek to offset */
  /* TODO: replace the fseek reference with our own implementation as fseek doesn't work in cc65 */
  if(myfseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    return -1;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, ENC_UTF8, NULL, NULL, NULL, &offset, TRUE)) {
    /* do nothing */
  }

  /* get current file position */
  fprintf(stdout, "%ld", offset);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
