#include "querycsv.h"

/* loop over each record in the result set, other than the first one */
void groupResultsInner(
    struct qryData *query,
    struct resultColumnValue *columns,
    int i
) {
  struct resultColumnValue *previousMatch;

  MAC_YIELD

  if(i == 0) {
    updateRunningCounts(query, columns, TRUE);

    if(query->recordCount > 1) {
      query->match = columns;
    }
    else {
      addGroupedResult(query, columns);
    }

    return;
  }

  previousMatch = query->match;

  /* if the current record to look at is identical to the previous one */
  /* if no group by clause then every record is part of one group */
  if(
      query->groupByClause == NULL ||
      recordCompare(
        (void *)previousMatch,
        (void *)columns,
        (void *)query
      ) != 0
    ) {
    addGroupedResult(query, previousMatch);

    updateRunningCounts(query, columns, TRUE);

    previousMatch = query->match = columns;
  }
  else {
    updateRunningCounts(query, columns, FALSE);

    cleanup_matchValues(query, &columns);
  }

  if(i == query->recordCount - 1) {
    addGroupedResult(query, previousMatch);
  }
}
