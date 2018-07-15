void outputHeader(struct qryData *query) {
  int firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  FILE *outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0) ? ", " : ",";

  MAC_YIELD

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
      fputsEncoded(
          (currentResultColumn->resultColumnName)+1,
          outputFile,
          query->outputEncoding
        );
    }
  }

  fputsEncoded(query->newLine, outputFile, query->outputEncoding);
}
