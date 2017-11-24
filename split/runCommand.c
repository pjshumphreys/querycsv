#include "querycsv.h"

void runCommand(struct qryData *query, char *inputText) {
  MAC_YIELD

  /* set up the output context */
  if(!outputSetup(query)) {
    query->CMD_RETVAL = EXIT_FAILURE;
  }
  else switch(query->commandMode) {
    case 1: {
      /* get the number of columns in a file */
      query->CMD_RETVAL = getColumnCount(query, inputText);
    } break;

    case 2: {
      /* get the file offset of the start of the next record in a file */
      query->CMD_RETVAL = getNextRecordOffset(query, inputText, query->CMD_OFFSET);
    } break;

    case 3: {
      /* get the unescaped value of column X of the record starting at the file offset */
      query->CMD_RETVAL = getColumnValue(
        query,
        inputText,
        query->CMD_OFFSET,
        query->CMD_COLINDEX);
    } break;

    case 4: {
      /* get the current date in ISO8601 format (local time with UTC offset) */
      query->CMD_RETVAL = getCurrentDate(query);
    } return;
  }

  freeAndZero(inputText);
}
