#include "querycsv.h"

void cleanup_query(struct qryData *query) {
  MAC_YIELD

  cleanup_columnReferences(query->columnReferenceHashTable);
  cleanup_resultColumns(query->firstResultColumn);
  cleanup_orderByClause(query->groupByClause);
  cleanup_orderByClause(query->orderByClause);
  cleanup_expression(query->joinsAndWhereClause);
  cleanup_inputTables(query->firstInputTable);

  if(query->outputFileName) {
    fclose(query->outputFile);
    free(query->outputFileName);
  }
}
