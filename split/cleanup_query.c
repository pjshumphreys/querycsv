#include "querycsv.h"

void cleanup_query(struct qryData * query)
{
  cleanup_columnReferences(query->columnReferenceHashTable);
  cleanup_resultColumns(query->firstResultColumn);
  cleanup_orderByClause(query->groupByClause);
  cleanup_orderByClause(query->orderByClause);
  cleanup_expression(query->joinsAndWhereClause);
  cleanup_inputTables(query->firstInputTable);
  free(query->intoFileName);

  //close the open files
  fclose(query->scratchpad);
}
