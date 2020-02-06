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
  }

  query->outputOffset = 0;

  /* if we aren't writing to stdout we may need or want to write a byte order mark */
  if(query->outputFile != stdout) {
    switch(query->outputEncoding) {
      case ENC_UTF8:
        /* only write the byte order mark if it was requested for utf-8 */
        if(!(query->params & PRM_BOM)) {
          break;
        }

      /* the 16 and 32 bit encodings always need a bom */
      case ENC_UTF16LE:
      case ENC_UTF16BE:
      case ENC_UTF32LE:
      case ENC_UTF32BE: {
        fputsEncoded("\xEF\xBB\xBF", query);
      }
    }
  }
  /* if we're printing the results, each environment can only
  correctly display its own print encoding */
  else {
    query->outputEncoding = ENC_PRINT;
  }

  return TRUE;
}
