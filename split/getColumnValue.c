int getColumnValue(
    struct qryData *query,
    char *inputFileName,
    long offset,
    int columnIndex
) {
  FILE *inputFile = NULL;
  char *output = NULL;
  size_t strSize = 0;
  int currentColumn = 0;
  struct inputTable table;
  int gotColumn;

  MAC_YIELD

  if(inputSeek(query, inputFileName, &offset, &inputFile) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  table.fileStream = inputFile;
  table.fileEncoding = query->CMD_ENCODING;
  table.cpIndex = table.arrLength = 0;

  /* initalise the "get a codepoint" data structures */
  getNextCodepoint(&table);

  /* get the text of the specified csv column (if available). */
  /* if it's not available we'll return an empty string */
  for(;
      currentColumn != columnIndex &&
      (gotColumn = getCsvColumn(&table, NULL, NULL, NULL, NULL, TRUE, query->newLine)) == TRUE;
      currentColumn++
  ) {
    /* get next column */
  }

  if(gotColumn) {
    getCsvColumn(&table, &output, &strSize, NULL, NULL, TRUE, query->newLine);
  }

  if(output) {
    /* output the value */
    fputsEncoded(output, query);

    /* free the string memory */
    freeAndZero(output);
  }

  if(query->outputFile == stdout) {
    fputsEncoded(query->newLine, query);
  }

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
