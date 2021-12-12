int getCurrentDate(char **retval) {
  char *output = NULL;
#if defined(__CC65__) || defined(__Z88DK)
  /*the commodore 64 doesn't have a real time clock, so why bother including time functions? */
  output = mystrdup("1970-01-01T00:00:00+0000");
#else
  time_t now;
  struct tm local;

  MAC_YIELD

  if(retval == NULL) {
    return FALSE;
  }

  /* get unix epoch seconds */
  time(&now);

  /* get localtime tm object and utc offset string */
  /* (we don't want to keep gmtime though as the users clock */
  /*  will probably be set relative to localtime) */
  if(d_tztime(&now, &local, NULL, &output) == FALSE) {
    fputs(TDB_TZTIMED_FAILED, stderr);

    freeAndZero(output);

    return FALSE;
  };

  /* place the utc offset in the output string. */
  /* %z unfortunately can't be used as it doesn't work properly */
  /* in some c library implementations (Watcom and MSVC) */
  if(
    d_sprintf(
      &output,
      "%d-%02d-%02dT%02d:%02d:%02d%s",
      local.tm_year + 1900,
      local.tm_mon + 1,
      local.tm_mday,
      local.tm_hour,
      local.tm_min,
      local.tm_sec,
      output
    ) == FALSE
  ) {
    fputs(TDB_SPRINTFD_FAILED, stderr);

    freeAndZero(output);

    return FALSE;
  };
#endif

  if(*retval) {
    free(*retval);
  }

  *retval = output;

  /* quit */
  return TRUE;
}
