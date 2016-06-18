#include "querycsv.h"

void groupResults(struct qryData * query, struct resultSet * results)
{
  struct resultSet resultsOrig;
  struct resultColumnValue * match;
  struct resultColumnValue * previousMatch;
  struct resultColumn * currentResultColumn;

  int i, len;

  //backup the original result set
  memcpy(&resultsOrig, results, sizeof(struct resultSet));

  //empty the result set
  results->recordCount = 0;
  results->records = NULL;

  //sort the records according to the group by clause
  if(query->groupByClause != NULL) {
    query->useGroupBy = TRUE;
    qsort_s(
      (void*)resultsOrig.records,
      resultsOrig.recordCount,
      (query->columnCount)*sizeof(struct resultColumnValue),
      recordCompare,
      (void*)query
    );
  }

  //keep a reference to the current record
  match = resultsOrig.records;

  //store a copy of the first record and initialise the running totals
  updateRunningCounts(query, match);

  //loop over each record in the result set, other than the first one
  for (i = 1, len = resultsOrig.recordCount; i <= len; i++) {
    previousMatch = match;
    match = &(resultsOrig.records[i*query->columnCount]);

    //if the current record to look at is identical to the previous one
    query->useGroupBy = TRUE;
    if(
        (query->groupByClause != NULL &&   //if no group by clause then every record is part of one group
        recordCompare(
          (void *)previousMatch,
          (void *)match,
          (void *)query
        ) != 0) ||
        i == len
      ) {
      //fix up the calculated columns that need it
      getGroupedColumns(query);

      //calculate remaining columns that make use of aggregation
      getCalculatedColumns(query, previousMatch, TRUE);

      //free the group text strings (to prevent heap fragmentation)
      cleanup_groupedColumns(query, previousMatch);

      //append the record to the new result set
      appendToResultSet(query, previousMatch, results);
    }

    if(i < len) {
      updateRunningCounts(query, match);
    }
  }

  //free the old result set
  free(resultsOrig.records);

  currentResultColumn = query->firstResultColumn;
  while(currentResultColumn != NULL) {
    currentResultColumn->groupingDone = TRUE;
    currentResultColumn = currentResultColumn->nextColumnInResults;
  }
}
