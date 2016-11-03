#include "querycsv.h"

void groupResults(struct qryData *query) {
  struct resultTree *resultsOrig;
  struct resultColumn *currentResultColumn;

  MAC_YIELD

  /* backup the original result set */
  resultsOrig = query->resultSet;
  query->resultSet = NULL;

  /* keep a reference to the current record */
  query->match = getFirstRecord(
    resultsOrig,
    query
  );

  /* store a copy of the first record and initialise the running totals */
  updateRunningCounts(query, query->match);

  tree_walkAndCleanup(
    query,
    &(resultsOrig),
    &groupResultsInner
  );

  /* mark each column as having had grouping performed */
  currentResultColumn = query->firstResultColumn;
  while(currentResultColumn != NULL) {
    currentResultColumn->groupingDone = TRUE;
    currentResultColumn = currentResultColumn->nextColumnInResults;
  }
}
