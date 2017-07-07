#include "querycsv.h"

void runCommand(char *string) {
  MAC_YIELD
/*
    //get the number of columns in a file
    if (strcmp(argv2[1], "--columns") == 0 && argc2 == 3) {
      return getColumnCount(argv2[2]);
    }

    //get the file offset of the start of the next record in a file
    else if (strcmp(argv2[1], "--next") == 0 && argc2 == 4) {
      return getNextRecordOffset(argv2[2], atol(argv2[3]));
    }

    //get the unescaped value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--value") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the unescaped trimmed value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--trimval") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the current date in ISO8601 format (local time with UTC offset)
    else if(strcmp(argv2[1], "--date") == 0 && argc2 == 3) {
      if(strcmp(argv2[2], "now") == 0) {
        return getCurrentDate();
      }
      else {
        fputs("not supported", stderr);

        return -1;
      }
    }

    //run a query
    else if(argc2 == 3) {
      return runQuery(argv2[2], argv2[1]);
    }
*/
}
