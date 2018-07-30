/* get localtime, gmtime and utc offset string from a time_t. allocate/free memory as needed */
/* any of the last three parameters can be skipped by passing null */
int d_tztime(
    time_t *now,
    struct tm *local,
    struct tm *utc,
    char **output
) {
  struct tm *lcl = NULL;
  struct tm *gm = NULL;

  int hourlcl;
  int hourutc;
  int difference;
  int hour_difference;
  int minute_difference;

  char *format = "+%02i%02i";

  MAC_YIELD

  if(now == NULL) {
    return FALSE;
  }

  if(local != NULL) {
    lcl = local;

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    reallocMsg((void*)&lcl, sizeof(struct tm));

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }

  if(utc != NULL) {
    gm = utc;

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    reallocMsg((void**)&gm, sizeof(struct tm));

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }

  if(output != NULL) {
    hourlcl = lcl->tm_hour;
    hourutc = gm->tm_hour;

    reallocMsg((void**)output, 6);

    if(lcl->tm_year > gm->tm_year) {
      hourlcl+=24;
    }
    else if(gm->tm_year > lcl->tm_year) {
      hourutc+=24;
    }
    else if(lcl->tm_yday > gm->tm_yday) {
      hourlcl+=24;
    }
    else if(gm->tm_yday > lcl->tm_yday) {
      hourutc+=24;
    }

    difference = ((hourlcl - hourutc)*60)+lcl->tm_min - gm->tm_min;
    minute_difference = difference % 60;
    hour_difference = (difference - minute_difference)/60;

    if(hour_difference < 0) {
      format = "%03i%02i";
    }

    sprintf(*output, format, hour_difference, minute_difference);
  }

  if(local == NULL) {
    free((void*)lcl);
  }

  if(utc == NULL) {
    free((void*)gm);
  }

  return TRUE;
}

/* format a date into a string. allocate/free memory as needed */
int d_strftime(char **ptr, char *format, struct tm *timeptr) {
  size_t length = 32; /* starting value */
  size_t length2 = 0;
  char *output = NULL;

  MAC_YIELD

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while(length2 == 0) {
    reallocMsg((void**)&output, length);

    /* calling strftime using the buffer we created */
    length2 = strftime(output, length, format, timeptr);

    /* double the allocation length to use if we need to try again */
    length *= 2;
  }

  /* shrink the allocated memory to fit the returned length */
  reallocMsg((void**)&output, length2+1);

  /* free the contents of ptr then update it to point to the string we've built up */
  freeAndZero(*ptr);
  *ptr = output;

  /* everything completed successfully */
  return TRUE;
}
