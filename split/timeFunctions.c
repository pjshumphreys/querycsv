
/* get localtime, gmtime and utc offset string from a time_t. allocate/free memory as needed */
/* any of the last three parameters can be skipped by passing null */
#if !(defined(__CC65__) || defined(__Z88DK))
const char format1[] = "+%02i%02i";
const char format2[] = "%03i%02i";
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

  char const *format = format1;

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
      format = format2;
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
#endif
