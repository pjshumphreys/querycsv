#include "querycsv.h"

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

  char *output2;
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
    lcl = (struct tm *)malloc(sizeof(struct tm));

    if(lcl == NULL) {
      return FALSE;
    }

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }

  if(utc != NULL) {
    gm = utc;

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    gm = (struct tm *)malloc(sizeof(struct tm));

    if(gm == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      return FALSE;
    }

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }

  if(output != NULL) {
    hourlcl = lcl->tm_hour;
    hourutc = gm->tm_hour;

      output2 = (char*)realloc((void*)*output, 6);

    if(output2 == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      if(utc == NULL) {
        free((void*)gm);
      }

      return FALSE;
    }
    *output = output2;

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
  char *output2 = NULL;

  MAC_YIELD

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while(length2 == 0) {
    output2 = realloc((void*)output, length*sizeof(char));

    if(output2 == NULL) {
      freeAndZero(output);

      return FALSE;
    }
    output = output2;

    /* calling strftime using the buffer we created */
    length2 = strftime(output, length, format, timeptr);

    /* double the allocation length to use if we need to try again */
    length *= 2;
  }

  /* shrink the allocated memory to fit the returned length */
  output2 = realloc((void*)output, (length2+1)*sizeof(char));

  /* quit if the shrinking didn't work successfully */
  if(output2 == NULL) {
    freeAndZero(output);

    return FALSE;
  }

  /* free the contents of ptr then update it to point to the string we've built up */
  freeAndZero(*ptr);
  *ptr = output2;

  /* everything completed successfully */
  return TRUE;
}
