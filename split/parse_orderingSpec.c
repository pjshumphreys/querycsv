#include "querycsv.h"

void parse_orderingSpec(
    struct qryData *queryData,
    struct expression *expressionPtr,
    int isDescending
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
  sortingListPtr->isDescending = isDescending;

  if(queryData->orderByClause == NULL) {
    sortingListPtr->nextInList = sortingListPtr;
    queryData->orderByClause = sortingListPtr;
  }
  else {
    sortingListPtr->nextInList = queryData->orderByClause->nextInList;
    queryData->orderByClause->nextInList = sortingListPtr;
    queryData->orderByClause = sortingListPtr;
  }
}
