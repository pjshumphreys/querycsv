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

    query->match = columns;

    return;
  }

  previousMatch = query->match;

  /* if the current record to look at is identical to the previous one */
  if(
      query->groupByClause == NULL ||   /* if no group by clause then every record is part of one group */
      recordCompare(
        (void *)previousMatch,
        (void *)columns,
        (void *)query
      ) != 0 ||
      i == query->recordCount - 1
    ) {
    /* fix up the calculated columns that need it */
    getGroupedColumns(query);

    /* calculate remaining columns that make use of aggregation */
    getCalculatedColumns(query, previousMatch, TRUE);

    query->groupCount++;

    /* append the record to the new result set */
    query->useGroupBy = FALSE;

    tree_insert(query, previousMatch, &(query->resultSet));

    query->useGroupBy = TRUE;

    if(i != query->recordCount - 1) {
      updateRunningCounts(query, columns, TRUE);

      query->match = columns;
    }
    else {
      cleanup_matchValues(query, &columns);
    }
  }
  else {
    updateRunningCounts(query, columns, FALSE);

    cleanup_matchValues(query, &columns);
  }
}
