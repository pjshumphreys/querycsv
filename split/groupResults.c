#include "querycsv.h"

void groupResults(struct qryData *query) {
  struct resultTree *resultsOrig;
  struct resultColumn *currentResultColumn;

  MAC_YIELD

  /* backup the original result set */
  resultsOrig = query->resultSet;
  query->resultSet = NULL;

  /* populate query->resultSet with the grouped results,
    while clearing up to memory of the original result set */
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
