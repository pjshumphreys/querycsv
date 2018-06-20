int getCurrentDate(struct qryData *query) {
  time_t now;
  struct tm local;
  char *output = NULL;

  MAC_YIELD

  /* get unix epoch seconds */
  time(&now);

  /* get localtime tm object and utc offset string */
  /* (we don't want to keep gmtime though as the users clock */
  /*  will probably be set relative to localtime) */
  if(d_tztime(&now, &local, NULL, &output) == FALSE) {
    fputs(TDB_TZTIMED_FAILED, stderr);

    freeAndZero(output);

    return EXIT_FAILURE;
  };

  /* place the utc offset in the output string. */
  /* %z unfortunately can't be used as it doesn't work properly */
  /* in some c library implementations (Watcom and MSVC) */
  if(d_sprintf(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    fputs(TDB_SPRINTFD_FAILED, stderr);

    freeAndZero(output);

    return EXIT_FAILURE;
  };

  /* place the rest of the time data in the output string */
  if(d_strftime(&output, output, &local) == FALSE) {
    fputs(TDB_STRFTIMED_FAILED, stderr);

    freeAndZero(output);

    return EXIT_FAILURE;
  }

  /* print the timestamp */
  fputsEncoded(output, query->outputFile, query->outputEncoding);

  /* free the string data */
  freeAndZero(output);

  /* quit */
  return EXIT_SUCCESS;
}
