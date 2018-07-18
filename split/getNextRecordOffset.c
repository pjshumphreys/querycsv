int getNextRecordOffset(
    struct qryData *query,
    char *inputFileName,
    long offset
) {
  FILE *inputFile = NULL;
  char* outText = NULL;
  struct inputTable table;

  MAC_YIELD

  if(inputSeek(query, inputFileName, offset, &inputFile) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  table.fileStream = inputFile;
  table.fileEncoding = query->CMD_ENCODING;
  table.cpIndex = table.arrLength = 0;

  /* initalise the "get a codepoint" data structures */
  getNextCodepoint(&table);

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&table, NULL, NULL, NULL, &offset, TRUE, query->newLine)) {
    /* do nothing */
  }

  /* get current file position */
  d_sprintf(&outText, "%ld", offset);

  fputsEncoded(outText, query->outputFile, query->outputEncoding);

  freeAndZero(outText);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
