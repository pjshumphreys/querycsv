
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

  /* write the byte order mark if it was requested */
  if(((query->params) & PRM_BOM) != 0) {
    fputs("\xEF\xBB\xBF", outputFile);
  }

  /* write column headers to the output file */
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputs(separator, outputFile);
      }
      else {
        firstColumn = FALSE;
      }

      /* strip over the leading underscore */
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }

  fputs(query->newLine, outputFile);
}
