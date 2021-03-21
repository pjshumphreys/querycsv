/* this creates a new hidden table column to store the expression, and another for other stuff to reference
  The expression populates the calculated column when getCalculatedColumns is run
*/

struct expression *parse_case(
    struct qryData *queryData,
    struct expression* simpleCompareTo,
    struct caseEntry* currentCase,
    struct expression* elseResult
) {
  struct expression* newExpression = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(elseResult != NULL) {
    currentCase = parse_when(
        queryData,
        currentCase,
        NULL, /* getCaseValue specifically checks for a null test then
        always returns the corresponding result */
        elseResult
      );
  }

  reallocMsg((void **)&newExpression, sizeof(struct expression));

  newExpression->type = EXP_CASE;
  newExpression->caseSensitive = FALSE;

  /* initial setup of expression values */
  if(simpleCompareTo != NULL) {
    newExpression->minTable = simpleCompareTo->minTable;
    newExpression->minColumn = simpleCompareTo->minColumn;
    newExpression->containsAggregates = simpleCompareTo->containsAggregates;
  }
  else {
    newExpression->minTable = 0;
    newExpression->minColumn = 0;
    newExpression->containsAggregates = FALSE;
  }

  newExpression->isNull = FALSE;
  newExpression->caseSensitive = FALSE;
  newExpression->value = NULL;

  /* cut the circularly linked list of currentCase */
  newExpression->unionPtrs.voidPtr = (void *)(currentCase->nextInList);
  currentCase->nextInList = NULL;

  /* loop over each case and update the minTable, minColumn and
  containsAggregates fields if necessary */
  currentCase = (struct caseEntry*)(newExpression->unionPtrs.voidPtr);

  while(currentCase != NULL) {
    if(currentCase->test != NULL) {
      if(currentCase->test->minTable > newExpression->minTable) {
        newExpression->minTable = currentCase->test->minTable;
        newExpression->minColumn = currentCase->test->minColumn;
      }

      if(currentCase->test->minColumn > newExpression->minColumn) {
        newExpression->minColumn = currentCase->test->minColumn;
      }

      if(currentCase->test->containsAggregates) {
        newExpression->containsAggregates = TRUE;
      }

      if(simpleCompareTo != NULL) {
        currentCase->test = parse_scalarExp(
            queryData,
            simpleCompareTo,
            EXP_EQ,
            currentCase->test
          );
      }
    }

    if(currentCase->value->minTable > newExpression->minTable) {
      newExpression->minTable = currentCase->value->minTable;
      newExpression->minColumn = currentCase->value->minColumn;
    }

    if(currentCase->value->minColumn > newExpression->minColumn) {
      newExpression->minColumn = currentCase->value->minColumn;
    }

    if(currentCase->value->containsAggregates) {
      newExpression->containsAggregates = TRUE;
    }

    currentCase = currentCase->nextInList;
  }

  return newExpression;
}

