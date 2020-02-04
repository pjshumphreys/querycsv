int inputSeek(
    struct qryData *query,
    char *inputFileName,
    long *offset,
    FILE **inputFile
) {
  long temp = 0;

  MAC_YIELD

  /* Attempt to open the input file and update the character encoding if the file has a BOM */
  *inputFile = skipBom(inputFileName, &temp, &(query->CMD_ENCODING));

  if(*inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  if(*offset == 0) {
    *offset = temp;
  }
  else {
    fclose(*inputFile);

    /* go directly to the specified offset if it's non zero */
    *inputFile = fopen(inputFileName, fopen_read);

    if(*inputFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      return EXIT_FAILURE;
    }

    /* seek to offset */
    if(myfseek(*inputFile, *offset, SEEK_SET) != 0) {
      fputs(TDB_COULDNT_SEEK, stderr);

      fclose(*inputFile);

      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
