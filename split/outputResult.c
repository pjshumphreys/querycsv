/* output a record to stdio */
void outputResult(
    struct qryData *query,
    struct resultColumnValue *columns,
    int currentIndex,
    struct resultTree *item
) {
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  int firstColumn = TRUE, j = 0;
  char *separator;
  char *string = NULL;
  char *string2 = NULL;

  MAC_YIELD

  fputsEncoded(query->newLine, query);

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
        fputsEncoded(separator, query);
      }
      else {
        firstColumn = FALSE;
      }

      field = &(columns[j]);

      if(field->isNull == TRUE) {
        if(query->params & PRM_POSTGRES) {
          fputsEncoded("\\N", query);
        }
        else if(query->params & PRM_NULL) {
          fputsEncoded("NULL", query);
        }
      }
      else {
        stringGet((unsigned char **)(&string), field, query->params);

        if(query->params & PRM_POSTGRES) {
          outputPostgresEscapes(string, query);
        }
        else if(
            (query->params & PRM_QUOTE) ||
            needsEscaping(string, query->params)
        ) {
          /* need to properly re-escape fields that need it */
          fputsEncoded("\"", query);
          if((string2 = strReplace("\"", "\"\"", string))) {
            fputsEncoded(string2, query);
          }
          fputsEncoded("\"", query);
          freeAndZero(string2);
        }
        else {
          fputsEncoded(string, query);
        }
      }
    }

    j++;
  }

  freeAndZero(string);

  if(currentIndex > -1) {
    cleanup_matchValues(query, &columns);
  }
}
