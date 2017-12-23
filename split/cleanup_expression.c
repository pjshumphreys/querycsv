void cleanup_expression(struct expression *currentExpression) {
  MAC_YIELD

  if(currentExpression != NULL) {
    switch(currentExpression->type) {
      case EXP_IN:
      case EXP_NOTIN:
        cleanup_expression(currentExpression->unionPtrs.inLeaves.leftPtr);
        cleanup_atomList(currentExpression->unionPtrs.inLeaves.lastEntryPtr);
      break;

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

      default:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
        cleanup_expression(currentExpression->unionPtrs.leaves.rightPtr);
      break;
    }

    free(currentExpression);
  }
}
