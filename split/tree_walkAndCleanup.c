/* Calls a callback function once for every item in the tree, and de-allocates memory used by the tree in the process */

/* This function only de-allocates/frees memory for the tree, ***but not the data items the tree contains*** */
void tree_walkAndCleanup(
    struct qryData *query,
    struct resultTree **root,
    void (*callback)(struct qryData *, struct resultColumnValue *, int)
) {
  struct resultTree *currentResult;
  struct resultTree *parentResult;
  int i = 0;

  MAC_YIELD

  if(root == NULL || *root == NULL) {
    return;
  }

  currentResult = *root;

  for( ; ; ) {
    if(currentResult->link[0]) {
      currentResult->link[0]->parent = currentResult;
      currentResult = currentResult->link[0];
      continue;
    }

    if((currentResult->type) != TRE_FREED) {
      callback(
        query,
        currentResult->columns,
        i
      );

      currentResult->type = TRE_FREED;
      i++;
    }

    if(currentResult->link[1]) {
      currentResult->link[1]->parent = currentResult;
      currentResult = currentResult->link[1];
      continue;
    }

    parentResult = currentResult->parent;

    if(parentResult != NULL) {
      if(parentResult->link[0] == currentResult) {
        currentResult = parentResult;
        free(currentResult->link[0]);
        currentResult->link[0] = NULL;
      }
      else {
        currentResult = parentResult;
        free(currentResult->link[1]);
        currentResult->link[1] = NULL;
      }
    }
    else {
      free(currentResult);
      *root = NULL;
      return;
    }
  }
}
