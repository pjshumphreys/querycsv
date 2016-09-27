#include "querycsv.h"

struct expression *parse_scalarExp(
    struct qryData *queryData,
    struct expression *leftPtr,
    int operator,
    struct expression *rightPtr
  ) {

  struct expression *expressionPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = operator;
  expressionPtr->value = NULL;

  if(rightPtr == NULL) {
    expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
    expressionPtr->unionPtrs.leaves.rightPtr = NULL;

    expressionPtr->minColumn = leftPtr->minColumn;
    expressionPtr->minTable = leftPtr->minTable;

    expressionPtr->containsAggregates = leftPtr->containsAggregates;
  }
  
  else if(rightPtr->minTable < leftPtr->minTable ||
    (rightPtr->minTable == leftPtr->minTable &&
    rightPtr->minColumn < leftPtr->minColumn)
  ) {
      
    //if the operator is commutable, then re-order so that the minimum table reference is on the left
    if(
        operator == EXP_PLUS ||
        operator == EXP_MULTIPLY ||
        operator == EXP_AND ||
        operator == EXP_OR
      ) {
      expressionPtr->unionPtrs.leaves.leftPtr = rightPtr;
      expressionPtr->unionPtrs.leaves.rightPtr = leftPtr;
    }
    else {
      expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
      expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
    }

    //The minimum table and column needed to evaluate this
    //sub-expression is the greater of the two operands
    expressionPtr->minColumn = leftPtr->minColumn;
    expressionPtr->minTable = leftPtr->minTable;

    expressionPtr->containsAggregates = (leftPtr->containsAggregates) || (rightPtr->containsAggregates);
  }
  else {
    expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
    expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
    expressionPtr->minColumn = rightPtr->minColumn;
    expressionPtr->minTable = rightPtr->minTable;

    expressionPtr->containsAggregates = (leftPtr->containsAggregates) || (rightPtr->containsAggregates);
  }

  return expressionPtr;
}

