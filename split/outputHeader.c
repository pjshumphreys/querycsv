void outputHeader(struct qryData *query) {
  int firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  char *separator;
  char *string2 = NULL;

  MAC_YIELD

  if((query->params & PRM_EURO)) {
    separator = (query->params & PRM_SPACE) ? "; " : ";";
  }
  else {
    separator = (query->params & PRM_SPACE) ? ", " : ",";
  }

  /* set up the output context */
  if(!outputSetup(query)) {
    return;
  }

  /* write column headers to the output file */
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

      /* strip over the leading underscore */
      if(query->params & PRM_POSTGRES) {
        outputPostgresEscapes((currentResultColumn->resultColumnName)+1, query);
      }
      else if(query->params & PRM_QUOTE) {
        fputsEncoded("\"", query);

        if((string2 = strReplace("\"", "\"\"", (currentResultColumn->resultColumnName)+1))) {
          fputsEncoded(string2, query);
        }

        fputsEncoded("\"", query);
        freeAndZero(string2);
      }
      else {
        fputsEncoded(
          (currentResultColumn->resultColumnName)+1,
          query
        );
      }
    }
  }
}
