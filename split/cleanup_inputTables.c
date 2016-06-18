#include "querycsv.h"

void cleanup_inputTables(struct inputTable * currentInputTable)
{
  struct inputTable* next;
  while(currentInputTable != NULL) {
    next = currentInputTable->nextInputTable;

    free(currentInputTable->queryTableName);
    fclose(currentInputTable->fileStream);
    free(currentInputTable);

    currentInputTable = next;
  }
}
