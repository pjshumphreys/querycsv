#include "querycsv.h"

int main(int argc, char *argv[]) {
  //supply a default temporary folder if none is present
  if(getenv(TEMP_VAR) == NULL) {
    putenv(DEFAULT_TEMP);
  }

  //supply some default timezone data if none is present
  if(getenv("TZ") == NULL) {
    putenv(TDB_DEFAULT_TZ);
  }

  //set the locale (among other things, this applies the
  //timezone data to the date functions)
  setlocale(LC_ALL, TDB_LOCALE);

  //identify whether to run a script or display the usage message
  if(argc == 2) {
    return runQuery(argv[1]);
  }

  //something else. print an error message and quit
  fputs(TDB_INVALID_COMMAND_LINE, stderr);

  return -1;
}
