int outputCurrentDate(struct qryData *query) {
  char *output = NULL;
  int retval;

  MAC_YIELD

  /* get the current date as a string */
  if((retval = getCurrentDate(&output))) {
    /* print the timestamp */
    fputsEncoded(output, query->outputFile, query->outputEncoding);
    fputsEncoded("\n", query->outputFile, query->outputEncoding);

    /* free the string data */
    freeAndZero(output);
  }

  /* quit */
  return retval == 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
