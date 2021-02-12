void groupResults(struct qryData *query) {
  struct resultTree *resultsOrig;
  struct resultColumn *currentResultColumn;

  MAC_YIELD

  /* backup the original result set */
  resultsOrig = query->resultSet;
  query->resultSet = NULL;

  /* reset the record count as we'll be counting the groups now */
  query->recordCount = 0;

  /* populate query->resultSet with the grouped results,
    while cleaning up the memory of the original result set */
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
