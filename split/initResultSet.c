#include "querycsv.h"

void initResultSet(struct qryData * query, struct resultColumnValue** match, struct resultSet* results)
{
  results->recordCount = 0;
  results->records = NULL;

  *match = NULL;

  reallocMsg(
    "couldn't initialise resultset",
    (void**)match,
    (query->columnCount)*sizeof(struct resultColumnValue)
  );
}
