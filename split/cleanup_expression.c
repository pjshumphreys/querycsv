void cleanup_expression(struct expression *currentExpression) {
  MAC_YIELD

  if(currentExpression != NULL) {
    switch(currentExpression->type) {
      case EXP_IN:
      case EXP_NOTIN: {
        cleanup_expression(currentExpression->unionPtrs.inLeaves.leftPtr);

        {
          struct atomEntry *currentAtom = currentExpression->unionPtrs.inLeaves.lastEntryPtr;
          struct atomEntry* next;

          while(currentAtom != NULL) {
            next = currentAtom->nextInList;

            free(currentAtom->content);
            free(currentAtom);

            currentAtom = next;
          }
        }
     } break;

      case EXP_COLUMN:
        /* the memory used by the input column will be freed elsewhere */
      break;

      case EXP_GROUP:
        /* the memory used here is cleaned up elsewhere */
      break;

      case EXP_LITERAL:
        free(currentExpression->unionPtrs.voidPtr);
      break;

      case EXP_CALCULATED:
        /* TODO: confirm that the expression will be freed elsewhere */
      break;

      case EXP_UMINUS:
      case EXP_UPLUS:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
      break;

      case EXP_CASE: {
        struct caseEntry *cases = (struct caseEntry *)(currentExpression->unionPtrs.voidPtr);
        struct caseEntry *currentCase;

        currentCase = cases;

        while(currentCase) {
          if(currentCase->test) {
            cleanup_expression(currentCase->test);
          }

          cleanup_expression(currentCase->value);
          cases = currentCase;

          currentCase = currentCase->nextInList;

          free(cases);
        }
      } break;

      default:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
        cleanup_expression(currentExpression->unionPtrs.leaves.rightPtr);
      break;
    }

    free(currentExpression);
  }
}
