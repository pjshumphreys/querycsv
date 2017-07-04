
#include "querycsv.h"

void outputHeader(
    struct qryData *query
  ) {
  int firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  FILE *outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";

  MAC_YIELD

  /* set up the output context */
  if(query->outputFileName) {
    query->newLine = "\r\n";
    query->outputFile = fopen(query->outputFileName, "wb");

    if (query->outputFile == NULL) {
      fputs("opening output file failed", stderr);
      return;
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

  /* write column headers to the output file */
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputsEncoded(separator, outputFile, query->outputEncoding);
      }
      else {
        firstColumn = FALSE;
      }

      /* strip over the leading underscore */
      fputsEncoded(
          (currentResultColumn->resultColumnName)+1,
          outputFile,
          query->outputEncoding
        );
    }
  }

  fputsEncoded(query->newLine, outputFile, query->outputEncoding);
}
