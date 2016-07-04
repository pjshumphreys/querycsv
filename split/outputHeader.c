
#include "querycsv.h"

void outputHeader(
    struct qryData *query
  ) {
  int recordsOutput = -1, i, j, len, firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  FILE* outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";

  //set up the output context
  if(query->intoFileName) {
    query->newLine = "\r\n";
    query->outputFile = fopen(query->intoFileName, "wb");

    if (query->outputFile == NULL) {
      fputs ("opening output file failed", stderr);
      return -1;
    }
  }
  else {
    query->outputFile = stdout;
    query->newLine = "\n";
  }

  outputFile = query->outputFile;

  //write the byte order mark if it was requested
  if(((query->params) & PRM_BOM) != 0) {
    fputs("\xEF\xBB\xBF", outputFile);
  }

  //write column headers to the output file
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

      //strip over the leading underscore
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }

  fputs(query->newLine, outputFile);
}
