#include "querycsv.h"

void cleanup_inputColumns(struct inputColumn *currentInputColumn) {
  struct inputColumn *next;

  MAC_YIELD

  while(currentInputColumn != NULL) {
    next = currentInputColumn->nextColumnInTable;

    free(currentInputColumn);

    currentInputColumn = next;
  }
}
