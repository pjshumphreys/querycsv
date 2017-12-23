void parse_groupingSpec(
    struct qryData *queryData,
    struct expression *expressionPtr
) {
  struct sortingList *sortingListPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return;
  }

  reallocMsg(
      (void**)&(sortingListPtr),
      sizeof(struct sortingList)
    );

  sortingListPtr->expressionPtr = expressionPtr;
  sortingListPtr->isDescending = FALSE;

  if(queryData->groupByClause == NULL) {
    sortingListPtr->nextInList = sortingListPtr;
    queryData->groupByClause = sortingListPtr;
  }
  else {
    sortingListPtr->nextInList = queryData->groupByClause->nextInList;
    queryData->groupByClause->nextInList = sortingListPtr;
    queryData->groupByClause = sortingListPtr;
  }
}


