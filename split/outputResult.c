#include "querycsv.h"

/* output a record to stdio */
void outputResult(
    struct qryData *query,
    struct resultColumnValue *columns,
    int currentIndex
  ) {
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  int firstColumn = TRUE, j;
  FILE *outputFile = query->outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";

  char *string = NULL;
  char *string2 = NULL;

  MAC_YIELD

  /* for output columns */
  j=0;
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

      field = &(columns[j]);

      switch(field->leftNull) {
        case TRUE: {
          if(((query->params) & PRM_EXPORT) != 0) {
            fputs("\\N", outputFile);
          }
        } break;

        default: {
          stringGet((unsigned char **)(&string), field, query->params);

          /* need to properly re-escape fields that need it */
          if(*string == '\0') {
            fputs("\"\"", outputFile);  /* empty string always needs escaping */
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
        } break;
      }
    }

    j++;
  }

  strFree(&string);
  cleanup_matchValues(query, &columns);

  fputs(query->newLine, outputFile);
}
