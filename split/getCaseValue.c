void getCaseValue(
    struct expression *expressionPtr,
    struct resultColumnParam *match
) {
  struct caseEntry * currentCase = ((struct caseEntry *)(expressionPtr->unionPtrs.voidPtr));

  MAC_YIELD

  while(
    currentCase->test != NULL &&
    walkRejectRecord(expressionPtr->minTable, currentCase->test, match)
  ) {
    currentCase = currentCase->nextInList;
  }

  if(currentCase->value->isNull) {
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
