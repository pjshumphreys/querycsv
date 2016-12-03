#include "querycsv.h"

void parse_whereClause(
    struct qryData *queryData,
    struct expression *expressionPtr
) {
  struct expression *currentClauses;
  struct expression *newClause;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return;
  }

  if(queryData->joinsAndWhereClause == NULL) {
    /* first join or where clause. just use it dirrectly */
    queryData->joinsAndWhereClause = expressionPtr;
  }
  else {
    /* get current clauses */
    currentClauses = queryData->joinsAndWhereClause;

    newClause = NULL;

    reallocMsg(
        (void**)&(newClause),
        sizeof(struct expression)
      );

    newClause->type = EXP_AND;
    newClause->value = NULL;

    if(
        expressionPtr->minTable < currentClauses->minTable ||
        (expressionPtr->minTable == currentClauses->minTable &&
        expressionPtr->minColumn < currentClauses->minColumn)
      ) {
      newClause->unionPtrs.leaves.leftPtr = expressionPtr;
      newClause->unionPtrs.leaves.rightPtr = currentClauses;

      /* The minimum table and column needed to evaluate this */
      /* sub-expression is the greater of the two operands */
      newClause->minColumn = currentClauses->minColumn;
      newClause->minTable = currentClauses->minTable;
    }
    else {
      newClause->unionPtrs.leaves.leftPtr = currentClauses;
      newClause->unionPtrs.leaves.rightPtr = expressionPtr;

      /* The minimum table and column needed to evaluate this */
      /* sub-expression is the greater of the two operands */
      newClause->minColumn = expressionPtr->minColumn;
      newClause->minTable = expressionPtr->minTable;
    }

    queryData->joinsAndWhereClause = newClause;
  }
}
