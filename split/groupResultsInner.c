#include "querycsv.h"

//loop over each record in the result set, other than the first one
void groupResultsInner(
    struct qryData *query,
    struct resultColumnValue *columns,
    int i
  ) {

  struct resultColumnValue *previousMatch;

  MAC_YIELD

  //loop over each record in the result set, other than the first one
  if(i) {
    previousMatch = query->match;
    query->match = columns;

    //if the current record to look at is identical to the previous one
    if(
        (query->groupByClause != NULL &&   //if no group by clause then every record is part of one group
        recordCompare(
          (void *)previousMatch,
          (void *)query->match,
          (void *)query
        ) != 0) ||
        i == query->recordCount
      ) {
      //fix up the calculated columns that need it
      getGroupedColumns(query);

      //calculate remaining columns that make use of aggregation
      getCalculatedColumns(query, previousMatch, TRUE);

      //free the group text strings (to prevent heap fragmentation)
      //cleanup_groupedColumns(query, previousMatch);

      query->useGroupBy = FALSE;

      //append the record to the new result set
      tree_insert(query, previousMatch, &(query->resultSet));

      query->useGroupBy = TRUE;
    }
    else {
      cleanup_matchValues(query, &previousMatch);
      free(previousMatch);
    }

    if(i < query->recordCount) {
      updateRunningCounts(query, query->match);
    }
  }
}
