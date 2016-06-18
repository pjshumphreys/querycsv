#include "querycsv.h"

void appendToResultSet(struct qryData * query, struct resultColumnValue* match, struct resultSet* results)
{
  reallocMsg(
    "couldn't append record to resultset",
    (void**)&(results->records),
    (results->recordCount+1)*(query->columnCount)*sizeof(struct resultColumnValue)
  );

  memcpy(
      &(results->records[results->recordCount*query->columnCount]),
      match,
      (query->columnCount)*sizeof(struct resultColumnValue)
    );

  results->recordCount++;
}
