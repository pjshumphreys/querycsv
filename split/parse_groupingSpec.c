#include "querycsv.h"

void parse_groupingSpec(
    struct qryData *queryData,
    struct expression *expressionPtr
  ) {
      
  struct sortingList *sortingListPtr = NULL;

  if(queryData->parseMode != 1) {
    return;
  }
  
  reallocMsg(
      "couldn't allocate sortingList entry",
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


