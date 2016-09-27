#include "querycsv.h"

void cleanup_inputColumns(struct inputColumn *currentInputColumn) {
  struct inputColumn *next;

  MAC_YIELD

  while(currentInputColumn != NULL) {
    next = currentInputColumn->nextColumnInTable;

    //free(currentInputColumn->fileColumnName); will be done by the column references instead
    free(currentInputColumn);

    currentInputColumn = next;
  }
}
