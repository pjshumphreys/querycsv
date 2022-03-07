void getCaseValue(
    struct expression *expressionPtr,
    struct resultColumnParam *match
) {
  struct caseEntry * currentCase = ((struct caseEntry *)(expressionPtr->unionPtrs.voidPtr));

  MAC_YIELD

  while(
    currentCase->test != NULL &&
    walkRejectRecord(expressionPtr->minTable, currentCase->test, match) &&
    currentCase->nextInList != NULL
  ) {
    currentCase = currentCase->nextInList;
  }

  if(currentCase == NULL || currentCase->value == NULL) {
    expressionPtr->isNull = TRUE;
    expressionPtr->value = NULL;
  }
  else if(currentCase->value->isNull) {
    freeAndZero(currentCase->value->value);
    expressionPtr->isNull = TRUE;
    expressionPtr->value = NULL;
  }
  else {
    getValue(currentCase->value, match);

    expressionPtr->isNull = FALSE;
    expressionPtr->value = currentCase->value->value;
    currentCase->value->value = NULL;
  }
}
