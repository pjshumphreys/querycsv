int getColumnCount(
    struct qryData *query,
    char *inputFileName
) {
  FILE *inputFile = NULL;
  int columnCount = 1;
  char * outText = NULL;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL, &(query->CMD_ENCODING));

  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, query->CMD_ENCODING, NULL, NULL, NULL, NULL, TRUE, query->newLine)) {
    columnCount++;
  }

  /* output the number of columns we counted */
  d_sprintf(&outText, "%d", columnCount);

  fputsEncoded(outText, query->outputFile, query->outputEncoding);

  freeAndZero(outText);

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
