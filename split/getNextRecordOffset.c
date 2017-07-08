#include "querycsv.h"

int getNextRecordOffset(
    struct qryData *query,
    char *inputFileName,
    long offset
) {
  FILE *inputFile = NULL;
  char* outText = NULL;
  int temp = 0;
  int encodingFromBom = ENC_UNKNOWN;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, &temp, &encodingFromBom);

  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  /* attempt to refine/identify the encoding of the input file */
  if(encodingFromBom != ENC_UNKNOWN && encodingFromBom != ENC_CP1047) {
    query->CMD_ENCODING = encodingFromBom;
  }
  else if(query->CMD_ENCODING == ENC_DEFAULT) {
    query->CMD_ENCODING = ENC_INPUT;
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
  d_sprintf(outText, "%ld", offset);

  fputsEncoded(outText, query->outputFile, query->outputEncoding);

  freeAndZero(outText);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
