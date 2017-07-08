#include "querycsv.h"

int getColumnValue(
    struct qryData *query,
    char *inputFileName,
    long offset,
    int columnIndex
  ) {

  FILE *inputFile = NULL;
  char *output = NULL;
  size_t strSize = 0;
  int currentColumn = 0;
  long temp = 0;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, &temp, &(query->CMD_ENCODING));

  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  if(offset == 0) {
    offset = temp;
  }
  else {
    fclose(inputFile);

    /* go directly to the specified offset if it's non zero */
    inputFile = fopen(inputFileName, "rb");

    if(inputFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      return EXIT_FAILURE;
    }

    /* seek to offset */
    if(myfseek(inputFile, offset, SEEK_SET) != 0) {
      fputs(TDB_COULDNT_SEEK, stderr);

      fclose(inputFile);

      return EXIT_FAILURE;
    }
  }

  /* get the text of the specified csv column (if available). */
  /* if it's not available we'll return an empty string */
  while(
        ++currentColumn != columnIndex ?
        getCsvColumn(&inputFile, query->CMD_ENCODING, NULL, NULL, NULL, NULL, TRUE):
        (getCsvColumn(&inputFile, query->CMD_ENCODING, &output, &strSize, NULL, NULL, TRUE) && FALSE)
      ) {
    /* get next column */
  }

  /* output the value */
  fputsEncoded(output, query->outputFile, query->outputEncoding);

  /* free the string memory */
  freeAndZero(output);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
