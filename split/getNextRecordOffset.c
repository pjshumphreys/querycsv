#include "querycsv.h"

int getNextRecordOffset(
    struct qryData *query,
    char *inputFileName,
    long offset
) {
  FILE *inputFile = NULL;
  char* outText = NULL;
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

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, query->CMD_ENCODING, NULL, NULL, NULL, &offset, TRUE)) {
    /* do nothing */
  }

  /* get current file position */
  d_sprintf(&outText, "%ld", offset);

  fputsEncoded(outText, query->outputFile, query->outputEncoding);

  freeAndZero(outText);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
