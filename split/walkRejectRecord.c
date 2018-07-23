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

  switch(expressionPtr->type) {
    case EXP_EQ:
    case EXP_NEQ:
    case EXP_LT:
    case EXP_LTE:
    case EXP_GT:
    case EXP_GTE: {
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
    } break;

    case EXP_AND: {
      return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) ||
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
    } break;

    case EXP_OR: {
      return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) &&
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
    } break;

    case EXP_COLUMN:
    case EXP_LITERAL:
    case EXP_CALCULATED:
    case EXP_PLUS:
    case EXP_MINUS:
    case EXP_MULTIPLY:
    case EXP_DIVIDE:
    case EXP_CONCAT:
    case EXP_UPLUS:
    case EXP_UMINUS:
    case EXP_NOT: {
      return walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) == 0;
    } break;

    case EXP_IN:
    case EXP_NOTIN: {
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
    } break;

    case EXP_ISNULL: {
      return !(expressionPtr->unionPtrs.leaves.leftPtr->isNull);
    } break;

    case EXP_NOTNULL: {
      return expressionPtr->unionPtrs.leaves.leftPtr->isNull;
    } break;
  }

  return FALSE;
}
