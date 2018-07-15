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
  char *separator = (((query->params) & PRM_SPACE) != 0) ? ", " : ",";
  char *string = NULL;
  char *string2 = NULL;

  MAC_YIELD

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

      switch(field->leftNull) { /* a null resulting from a left join */
        case TRUE: {
          if(((query->params) & PRM_EXPORT) != 0) {
            fputsEncoded("\\N", outputFile, query->outputEncoding);
          }
        } break;

        default: {
          stringGet((unsigned char **)(&string), field, query->params);

          /* need to properly re-escape fields that need it */
          if(*string == '\0') {
            fputsEncoded("\"\"", outputFile, query->outputEncoding);  /* empty string always needs escaping */
          }
          else if(needsEscaping(string, query->params)) {
            string2 = strReplace("\"", "\"\"", string);
            fputsEncoded("\"", outputFile, query->outputEncoding);
            fputsEncoded(string2, outputFile, query->outputEncoding);
            fputsEncoded("\"", outputFile, query->outputEncoding);
            freeAndZero(string2);
          }
          else {
            fputsEncoded(string, outputFile, query->outputEncoding);
          }
        } break;
      }
    }

    j++;
  }

  freeAndZero(string);

  if(currentIndex > -1) {
    cleanup_matchValues(query, &columns);
  }

  query->recordCount++;

  fputsEncoded(query->newLine, outputFile, query->outputEncoding);
}
