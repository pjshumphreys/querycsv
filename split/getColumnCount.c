#include "querycsv.h"

int getColumnCount(
    struct qryData *query,
    char *inputFileName
) {
  FILE *inputFile = NULL;
  int columnCount = 1;
  char * outText = NULL;
  int encodingFromBom = ENC_UNKNOWN;


  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL, &encodingFromBom);

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

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, query->CMD_ENCODING, NULL, NULL, NULL, NULL, TRUE)) {
    columnCount++;
  }

  /* output the number of columns we counted */
  d_sprintf(outText, "%d", columnCount);

  fputsEncoded(outText, query->outputFile, query->outputEncoding);

  freeAndZero(outText);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
