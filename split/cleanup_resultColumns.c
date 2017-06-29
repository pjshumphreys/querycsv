#include "querycsv.h"

void cleanup_resultColumns(struct resultColumn *currentResultColumn) {
  struct resultColumn *next;

  MAC_YIELD

  while(currentResultColumn != NULL) {
    next = currentResultColumn->nextColumnInResults;

    free(currentResultColumn->resultColumnName);
    free(currentResultColumn);

    currentResultColumn = next;
  }
}
