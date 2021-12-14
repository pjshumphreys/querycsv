int getColumnCount(
    struct qryData *query,
    char *inputFileName
) {
  FILE *inputFile = NULL;
  int columnCount = 1;
  char * outText = NULL;
  struct inputTable table;
  long offset = query->CMD_PARAMS & PRM_HEADER ? 128 : 0;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, &offset, &(query->CMD_ENCODING));

  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  table.fileStream = inputFile;
  table.fileEncoding = query->CMD_ENCODING;
  table.cpIndex = table.arrLength = 0;

  /* initalise the "get a codepoint" data structures */
  getNextCodepoint(&table);

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&table, NULL, NULL, NULL, NULL, TRUE, query->newLine)) {
    columnCount++;
  }

  /* output the number of columns we counted */
  d_sprintf(&outText, D_STRING, columnCount);

  fputsEncoded(outText, query);

  freeAndZero(outText);

  if(query->outputFile == stdout) {
    fputsEncoded(query->newLine, query);
  }

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
