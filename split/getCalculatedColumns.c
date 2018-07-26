void getCalculatedColumns(
    struct qryData *query,
    struct resultColumnValue *match,
    int runAggregates
) {
  int i, j;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;
  struct columnRefHashEntry *currentHashEntry;
  struct resultColumnParam matchParams;
  struct expression *currentExpression;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            currentReference->reference.calculatedPtr.expressionPtr->containsAggregates == runAggregates &&

            /* get the current result column */
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL
          ) {

          /* start setting column value fields */
          j = currentResultColumn->resultColumnIndex;

          currentExpression = currentReference->reference.calculatedPtr.expressionPtr;

          /* get expression value for this match */
          if(currentExpression->type != EXP_CASE) {
            getValue(currentExpression, &matchParams);
          }
          else {
            getCaseValue(currentExpression, &matchParams);
          }

          /* store the value's length */
          if(currentExpression->isNull) {
            match[j].length = 0;
            match[j].isNull = TRUE;
            match[j].value = mystrdup("");
          }
          else {
            match[j].isNull = FALSE;
            match[j].length = strlen(currentExpression->value);
            match[j].value = currentExpression->value;
          }

          /* free the expression value for this match */
          /* strFree(&(currentReference->reference.calculatedPtr.expressionPtr->value)); */
          currentExpression->value = NULL;
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      /* go to the next reference in the hash table */
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
