void runCommand(struct qryData *query) {
  MAC_YIELD

  /* set up the output context */
  if(!outputSetup(query)) {
    query->CMD_RETVAL = EXIT_FAILURE;
  }
  else switch(query->commandMode) {
    case 1: {
      /* get the number of columns in a file */
      query->CMD_RETVAL = getColumnCount(query, query->inputFileName);
    } break;

    case 2: {
      /* get the file offset of the start of the next record in a file */
      query->CMD_RETVAL = getNextRecordOffset(query, query->inputFileName, query->CMD_OFFSET);
    } break;

    case 3: {
      /* get the unescaped value of column X of the record starting at the file offset */
      query->CMD_RETVAL = getColumnValue(
        query,
        query->inputFileName,
        query->CMD_OFFSET,
        query->CMD_COLINDEX);
    } break;

    case 4: {
      /* get the current date in ISO8601 format (local time with UTC offset) */
      char *output = NULL;
      int retval;

      /* get the current date as a string */
      if((retval = getCurrentDate(&output))) {
        /* print the timestamp */
        fputsEncoded(output, query);

        if(query->outputFile == stdout) {
          fputsEncoded(query->newLine, query);
        }

        /* free the string data */
        freeAndZero(output);
      }

      /* quit */
      query->CMD_RETVAL = (retval == 0 ? EXIT_FAILURE : EXIT_SUCCESS);
    } return;

    case 5: {
      /* just output the entire file that's specified, so we can convert between character encodings */
      query->CMD_RETVAL = outputFile(query, query->inputFileName);
    } break;
  }

  freeAndZero(query->inputFileName);
}
