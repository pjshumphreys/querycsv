int outputFile(
    struct qryData *query,
    char *inputFileName
) {
  FILE *inputFile = NULL;
  char * outText = NULL;
  struct inputTable table;
  size_t byteLength = 0;
  long temp = query->CMD_PARAMS & PRM_HEADER ? 128 : 0;
  long c;
  int batched = 0;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, &temp, &(query->CMD_ENCODING));

  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  table.fileStream = inputFile;
  table.fileEncoding = query->CMD_ENCODING;
  table.cpIndex = table.arrLength = 0;

  /* initalise the "get a codepoint" data structures */
  getNextCodepoint(&table);

  /* read bytes until end of file occurs */
  while((c = getCurrentCodepoint(&table, NULL)) != MYEOF) {
    /* perform newline translation */
    switch(c) {
      case 0x0D:
      case 0x0A:
      case 0x85: {
        getNextCodepoint(&table);

        if(c == 0x0D && (c = getCurrentCodepoint(&table, NULL)) == 0x0A) {
          getNextCodepoint(&table);
        }

        if(batched) {
          strAppend(0, &outText, &byteLength);
          fputsEncoded(outText, query);
          freeAndZero(outText);
          byteLength = 0;
          batched = 0;
        }

        fputsEncoded(query->newLine, query);
        query->codepointsInLine = 0;
      } continue;
    }

    strAppendUTF8(c, (unsigned char **)(&outText), &byteLength);
    batched++;

    if(batched > 79) {
      strAppend(0, &outText, &byteLength);
      fputsEncoded(outText, query);
      freeAndZero(outText);
      byteLength = 0;
      batched = 0;
    }

    getNextCodepoint(&table);
  }

  if(batched) {
    strAppend(0, &outText, &byteLength);
    fputsEncoded(outText, query);
    freeAndZero(outText);
    byteLength = 0;
  }

  if(query->outputFile == stdout) {
    fputs(query->newLine, stdout);
  }

  /* close the input file and return */
  fclose(inputFile);

  return EXIT_SUCCESS;
}
