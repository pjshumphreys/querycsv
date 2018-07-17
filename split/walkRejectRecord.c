/* TRUE means the record was rejected.  FALSE means the record should not yet be rejected */
int walkRejectRecord(
    int currentTable,
    struct expression *expressionPtr,
    struct resultColumnParam *match
) {
  int retval, i;
  struct atomEntry *currentAtom;

  MAC_YIELD

  if(expressionPtr == NULL || currentTable < expressionPtr->minTable) {
    return FALSE;
  }

  if(expressionPtr->type >= EXP_EQ && expressionPtr->type <= EXP_GTE) {
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
    getValue(expressionPtr->unionPtrs.leaves.rightPtr, match);

    if(
        expressionPtr->unionPtrs.leaves.leftPtr->isNull ||
        expressionPtr->unionPtrs.leaves.rightPtr->isNull
      ) {

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);

      return FALSE;
    }

    retval = strCompare(
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.rightPtr->value)),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
        (void (*)(void))&getUnicodeChar,
        (void (*)(void))&getUnicodeChar
      );

    freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);

    switch(expressionPtr->type) {
      case EXP_EQ:
        return retval != 0;
      case EXP_NEQ:
        return retval == 0;
      case EXP_LT:
        return retval != -1;
      case EXP_LTE:
        return retval == 1;
      case EXP_GT:
        return retval != 1;
      case EXP_GTE:
        return retval == -1;
    }
  }
  else if(expressionPtr->type == EXP_AND) {
    return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) ||
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type == EXP_OR) {
    return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) &&
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type <= EXP_NOT) {
    return walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) == 0;
  }
  else if(expressionPtr->type == EXP_IN || expressionPtr->type == EXP_NOTIN) {
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);

    if(expressionPtr->unionPtrs.leaves.leftPtr->isNull) {
      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      return FALSE;
    }

    for(
        i = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->index,
          currentAtom = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->nextInList;
        i != currentAtom->index;
        currentAtom = currentAtom->nextInList
      ) {
      if(strCompare(
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
        (unsigned char **)(&(currentAtom->content)),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
        (void (*)(void))&getUnicodeChar,
        (void (*)(void))&getUnicodeChar
      ) == 0) {
        freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
        return expressionPtr->type == EXP_NOTIN;  /* FALSE */
      }
    }

    freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    return expressionPtr->type != EXP_NOTIN;  /* TRUE */
  }

  return FALSE;
}
