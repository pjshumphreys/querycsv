#include "querycsv.h"

void runCommand(struct qryData *query, char *inputText) {
  FILE *outputFile;

  MAC_YIELD

  /* set up the output context */
  if(query->outputFileName) {
    query->newLine = "\r\n";
    query->outputFile = fopen(query->outputFileName, "wb");

    if(query->outputFile == NULL) {
      fputs(TDB_OUTPUT_FAILED, stderr);
      query->CMD_RETVAL = EXIT_FAILURE;
    }
  }
  else {
    query->outputFile = stdout;
    query->newLine = "\n";
  }

  outputFile = query->outputFile;

  /* if we aren't writing to stdout we may need or want to write a byte order mark */
  if(outputFile != stdout) {
    switch(query->outputEncoding) {
      case ENC_UTF8:
        /* only write the byte order mark if it was requested for utf-8 */
        if(((query->params) & PRM_BOM) == 0) {
          break;
        }

      /* the 16 and 32 bit encodings always need a bom */
      case ENC_UTF16LE:
      case ENC_UTF16BE:
      case ENC_UTF32LE:
      case ENC_UTF32BE: {
        fputsEncoded("\xEF\xBB\xBF", outputFile, query->outputEncoding);
      }
    }
  }
  /* if we're printing the results, each environment can only
  correctly display its own print encoding */
  else {
    query->outputEncoding = ENC_PRINT;
  }

  switch(query->commandMode) {
    case 1: {
      /* get the number of columns in a file */
      query->CMD_RETVAL = getColumnCount(query, inputText);
    } break;

    case 2: {
      /* get the file offset of the start of the next record in a file */
      query->CMD_RETVAL = getNextRecordOffset(query, inputText, query->CMD_OFFSET);
    } break;

    case 3: {
      /* get the unescaped value of column X of the record starting at the file offset */
      query->CMD_RETVAL = getColumnValue(
        query,
        inputText,
        query->CMD_OFFSET,
        query->CMD_COLINDEX);
    } break;

    case 4: {
      /* get the current date in ISO8601 format (local time with UTC offset) */
      query->CMD_RETVAL = getCurrentDate(query);
    } return;
  }

  freeAndZero(inputText);
}
