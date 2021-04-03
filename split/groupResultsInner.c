/* loop over each record in the result set, other than the first one */
void groupResultsInner(
    struct qryData *query,
    struct resultColumnValue *columns,
    int i,
    struct resultTree *item
) {
  struct resultTree *tempItem;

  MAC_YIELD

  if(item->type == TRE_SKIP) {
    cleanup_matchValues(query, &columns);
  }
  else {
    query->groupCount = 0;

    updateRunningCounts(query, item, NULL);

    if(query->hasGrouping) {
      /* look at the next item */
      tempItem = item->link[1];

      /* while the next nth result is part of the same group
      as this result, add to the number of items to look ahead */
      while(
        tempItem &&
        recordCompare(
          (void *)columns,
          (void *)tempItem->columns,
          (void *)query
        ) == 0
      ) {
        tempItem->type = TRE_SKIP;
        updateRunningCounts(query, tempItem, item);
        tempItem = tempItem->link[1];
      }
    }

    addGroupedResult(query, columns);
  }
}
