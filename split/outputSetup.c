int outputSetup(struct qryData *query) {
  MAC_YIELD

  /* set up the output context */
  if(query->outputFileName) {
    query->outputFile = fopen(query->outputFileName, fopen_write);

    if(query->outputFile == NULL) {
      fputs(TDB_OUTPUT_FAILED, stderr);
      return FALSE;
    }
  }
  else {
    query->outputFile = stdout;

    /* if we're printing the results, each environment can only
    correctly display its own print encoding */
    query->outputEncoding = ENC_PRINT;
  }

  query->outputOffset = 0;

  return TRUE;
}
