void outputHeader(struct qryData *query) {
  int firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  FILE *outputFile;
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

  outputFile = query->outputFile;

  /* write column headers to the output file */
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

      /* strip over the leading underscore */
      if(query->params & PRM_POSTGRES) {
        outputPostgresEscapes((currentResultColumn->resultColumnName)+1, outputFile, query->outputEncoding);
      }
      else {
        if(query->params & PRM_QUOTE) {
          fputsEncoded("\"", outputFile, query->outputEncoding);

          if((string2 = strReplace("\"", "\"\"", (currentResultColumn->resultColumnName)+1))) {
            fputsEncoded(string2, outputFile, query->outputEncoding);
          }

          fputsEncoded("\"", outputFile, query->outputEncoding);
          freeAndZero(string2);
        }
        else {
          fputsEncoded(
            (currentResultColumn->resultColumnName)+1,
            outputFile,
            query->outputEncoding
          );
        }
      }
    }
  }
}
