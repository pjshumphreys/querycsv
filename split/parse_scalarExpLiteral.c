#include "querycsv.h"

struct expression *parse_scalarExpLiteral(
    struct qryData *queryData,
    char *literal
  ) {

  struct expression* expressionPtr = NULL;
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = EXP_LITERAL;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.voidPtr = strdup(literal);
  expressionPtr->minColumn = 0;
  expressionPtr->minTable = 0;

  expressionPtr->containsAggregates = FALSE;

  return expressionPtr;
}
