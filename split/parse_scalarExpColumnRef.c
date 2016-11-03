#include "querycsv.h"

/*produces a different node type depending on whether a csv column is being referered to*/
struct expression *parse_scalarExpColumnRef(
    struct qryData *queryData,
    struct columnReference *referencePtr
  ) {

  struct expression *expressionPtr = NULL, *expressionColumnPtr;
  struct inputColumn *columnPtr = NULL;

  MAC_YIELD
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->value = NULL;

  if(referencePtr->referenceType == REF_COLUMN) {
    expressionPtr->type = EXP_COLUMN;

    /* unbox csv column reference */
    columnPtr = referencePtr->reference.columnPtr;
    
    expressionPtr->unionPtrs.voidPtr = (void *)(columnPtr);
    expressionPtr->minColumn = columnPtr->columnIndex;
    expressionPtr->minTable = ((struct inputTable*)columnPtr->inputTablePtr)->fileIndex;

    /* this line might need to be changed */
    expressionPtr->containsAggregates = FALSE;
  }
  else {
    expressionPtr->type = EXP_CALCULATED;

    /* unbox calculated expression column */
    expressionColumnPtr = referencePtr->reference.calculatedPtr.expressionPtr;

    /* TODO: fill out these 3 fields with the correct value */
    expressionPtr->unionPtrs.voidPtr = (void *)(expressionColumnPtr);
    expressionPtr->minColumn = expressionColumnPtr->minColumn;
    expressionPtr->minTable = expressionColumnPtr->minTable;

    /* this line might need to be changed */
    expressionPtr->containsAggregates = expressionColumnPtr->containsAggregates;
  }
  
  return expressionPtr;
}
