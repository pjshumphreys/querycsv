#include "querycsv.h"

int outputResults(struct qryData * query, struct resultSet * results)
{
  int recordsOutput = -1, i, j, len, firstColumn = TRUE;
  FILE * outputFile = NULL;
  struct resultColumn* currentResultColumn;
  struct resultColumnValue* field;

  char *string = NULL;
  char *string2 = NULL;
  char *string3 = (((query->params) & PRM_SPACE) != 0)?",":", ";
  char *newline;

  if(query->intoFileName) {
    outputFile = fopen(query->intoFileName, "wb");
    newline = "\r\n";
    if (outputFile == NULL) {
      fputs ("opening output file failed", stderr);
      return -1;
    }
  }
  else {
    newline = "\n";
    outputFile = stdout;
  }

  if(((query->params) & PRM_BOM) != 0) {
    fputs("\xEF\xBB\xBF", outputFile);
  }

  firstColumn = TRUE;

  //write column headers to the output file
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputs(string3, outputFile);
      }
      else {
        firstColumn = FALSE;
      }

      //strip over the leading underscore
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }

  fputs(newline, outputFile);

  //dereference the string offsets and lengths and write
  //to the output file. remove any that were there just for sorting purposes

  //for records
  for (i = 0, len = results->recordCount; i != len; i++) {
    firstColumn=TRUE;

    //for output columns
    j=0;
    for(
        currentResultColumn = query->firstResultColumn;
        currentResultColumn != NULL;
        currentResultColumn = currentResultColumn->nextColumnInResults
      ) {

      if(currentResultColumn->isHidden == FALSE) {
        if (!firstColumn) {
          fputs(string3, outputFile);
        }
        else {
          firstColumn = FALSE;
        }

        field = &(results->records[(i*(query->columnCount))+j]);

        switch(field->leftNull) {
          case TRUE:
            if(((query->params) & PRM_EXPORT) != 0) {
              fputs("\\N", outputFile);
            }
          break;

          default:
            stringGet(&string, field, query->params);

            //need to properly re-escape fields that need it
            if(*string == '\0') {
              fputs("\"\"", outputFile);  //empty string always needs escaping
            }
            else if(needsEscaping(string, query->params)) {
              string2 = strReplace("\"","\"\"", string);
              fputs("\"", outputFile);
              fputs(string2, outputFile);
              fputs("\"", outputFile);
              strFree(&string2);
            }
            else {
              fputs(string, outputFile);
            }
          break;
        }
      }

      j++;
    }

    fputs(newline, outputFile);
  }

  //close the output file
  if(query->intoFileName) {
    fclose(outputFile);
    recordsOutput = results->recordCount;
  }

  //free used memory
  strFree(&string);

  return recordsOutput;
}
