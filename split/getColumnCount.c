#include "querycsv.h"

int getColumnCount(char *inputFileName) {
  FILE *inputFile = NULL;
  int columnCount = 1;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, ENC_UTF8, NULL, NULL, NULL, NULL, TRUE)) {
    columnCount++;
  }

  /* output the number of columns we counted */
  fprintf(stdout, "%d", columnCount);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
