#include "querycsv.h"

int getColumnValue(
    char *inputFileName,
    long offset,
    int columnIndex
  ) {

  FILE *inputFile = NULL;
  char *output = (char*)malloc(1);
  size_t strSize = 0;
  int currentColumn = 0;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    freeAndZero(output);
    return -1;
  }

  /* seek to offset */
  /* TODO. replace the fseek reperence with our own implementation as fseek doesn't work in cc65 */
  if(myfseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    freeAndZero(output);
    return -1;
  }

  /* get the text of the specified csv column (if available). */
  /* if it's not available we'll return an empty string */
  while(
        ++currentColumn != columnIndex ?
        getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE):
        (getCsvColumn(&inputFile, &output, &strSize, NULL, NULL, TRUE) && FALSE)
      ) {
    /* get next column */
  }

  /* output the value */
  fputs(output, stdout);

  /* free the string memory */
  freeAndZero(output);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
