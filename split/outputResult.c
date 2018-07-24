/* output a record to stdio */
void outputResult(
    struct qryData *query,
    struct resultColumnValue *columns,
    int currentIndex
) {
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  int firstColumn = TRUE, j = 0;
  FILE *outputFile = query->outputFile;
  char *separator;
  char *string = NULL;
  char *string2 = NULL;

  MAC_YIELD

  if(query->params & PRM_EURO) {
    separator = (query->params & PRM_SPACE) ? "; " : ";";
  }
  else {
    separator = (query->params & PRM_SPACE) ? ", " : ",";
  }

  /* for output columns */
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if(!firstColumn) {
        fputsEncoded(separator, outputFile, query->outputEncoding);
      }
      else {
        firstColumn = FALSE;
      }

      field = &(columns[j]);

      if(field->isNull == TRUE) {
        if(query->params & PRM_POSTGRES) {
          fputsEncoded("\\N", outputFile, query->outputEncoding);
        }
        else if(query->params & PRM_NULL) {
          fputsEncoded("NULL", outputFile, query->outputEncoding);
        }
      }
      else {
        stringGet((unsigned char **)(&string), field, query->params);

        if(query->params & PRM_POSTGRES) {
          outputPostgresEscapes(string, outputFile, query->outputEncoding);
        }
        else {
          /* need to properly re-escape fields that need it */
          if(
              (query->params & PRM_QUOTE) ||
              needsEscaping(string, query->params)
          ) {
            fputsEncoded("\"", outputFile, query->outputEncoding);
            if((string2 = strReplace("\"", "\"\"", string))) {
              fputsEncoded(string2, outputFile, query->outputEncoding);
            }
            fputsEncoded("\"", outputFile, query->outputEncoding);
            freeAndZero(string2);
          }
          else {
            fputsEncoded(string, outputFile, query->outputEncoding);
          }
        }
      }
    }

    j++;
  }

  freeAndZero(string);

  if(currentIndex > -1) {
    cleanup_matchValues(query, &columns);
  }

  fputsEncoded(query->newLine, outputFile, query->outputEncoding);
}
