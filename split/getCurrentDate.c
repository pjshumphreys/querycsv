#include "querycsv.h"

int getCurrentDate()
{
  time_t now;
  struct tm local;

  char *output = NULL;

  //get unix epoch seconds
  time(&now);

  //get localtime tm object and utc offset string
  //(we don't want to keep gmtime though as the users clock
  // will probably be set relative to localtime)
  if(d_tztime(&now, &local, NULL, &output) == FALSE) {
    fputs(TDB_TZTIMED_FAILED, stderr);

    strFree(&output);

    return -1;
  };

  //place the utc offset in the output string.
  //%z unfortunately can't be used as it doesn't work properly
  //in some c library implementations (Watcom and MSVC)
  if(d_sprintf(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    fputs(TDB_SPRINTFD_FAILED, stderr);

    strFree(&output);

    return -1;
  };

  //place the rest of the time data in the output string
  if(d_strftime(&output, output, &local) == FALSE) {
    fputs(TDB_STRFTIMED_FAILED, stderr);

    strFree(&output);

    return -1;
  }

  //print the timestamp
  fputs(output, stdout);

  //free the string data
  strFree(&output);

  //quit
  return 0;
}
