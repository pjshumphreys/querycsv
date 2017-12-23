int getNextRecordOffset(
    struct qryData *query,
    char *inputFileName,
    long offset
) {
  FILE *inputFile = NULL;
  char* outText = NULL;

  MAC_YIELD

  if(inputSeek(query, inputFileName, offset, &inputFile) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, query->CMD_ENCODING, NULL, NULL, NULL, &offset, TRUE)) {
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
