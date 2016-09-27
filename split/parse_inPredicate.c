#include "querycsv.h"

struct expression *parse_inPredicate(
    struct qryData *queryData,
    struct expression *leftPtr,
    int isNotIn,
    struct atomEntry *lastEntryPtr
  ) {

  struct expression *expressionPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = isNotIn==TRUE?EXP_NOTIN:EXP_IN;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.inLeaves.leftPtr = leftPtr;
  expressionPtr->unionPtrs.inLeaves.lastEntryPtr = lastEntryPtr;
  expressionPtr->minColumn = leftPtr->minColumn;
  expressionPtr->minTable = leftPtr->minTable;

  expressionPtr->containsAggregates = leftPtr->containsAggregates;

  return expressionPtr;
}

