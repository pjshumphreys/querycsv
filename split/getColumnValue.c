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

  MAC_YIELD

  if(inputSeek(query, inputFileName, offset, &inputFile) == EXIT_FAILURE) {
    return EXIT_FAILURE;
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
