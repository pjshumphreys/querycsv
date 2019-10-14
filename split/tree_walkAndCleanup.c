/* Calls a callback function once for every item in the tree, and de-allocates memory used by the tree in the process */

/* This function only de-allocates/frees memory for the tree, ***but not the data items the tree contains*** */
void tree_walkAndCleanup(
    struct qryData *query,
    struct resultTree **root,
    void (*callback)(struct qryData *, struct resultColumnValue *, int, struct resultTree *)
) {
  struct resultTree *currentResult;
  struct resultTree *tempResult;
  int i = 0;

  MAC_YIELD

  if(root == NULL || *root == NULL) {
    return;
  }

  currentResult = *root;
  *root = NULL;

  /*
    Convert the binary tree to a doubly linked list so that we can easily
    navigate backwards and forwards within the result set.
    We need to be able to do this in order to get select distinct to work properly
  */
  /* link[0] becomes the previous result in the list */
  /* link[1] becomes the next result in the list */
  do {
    if(currentResult->link[0] && currentResult->link[0]->type != TRE_CONVERTED) {
      currentResult->link[0]->parent = currentResult;
      currentResult = currentResult->link[0];
      continue;
    }
    else if(currentResult->link[1] && currentResult->link[1]->type != TRE_CONVERTED) {
      currentResult->link[1]->parent = currentResult;
      currentResult = currentResult->link[1];
      continue;
    }

    if(*root == NULL) {
      if(currentResult->parent && currentResult->parent->link[1] == currentResult) {
       *root = currentResult->parent;
      }
      else {
        *root = currentResult;
      }
    }

    currentResult->type = TRE_CONVERTED;

    if(currentResult->link[0]) {
      currentResult->link[0]->link[1] = currentResult;
    }

    if(currentResult->link[1]) {
      if(currentResult->parent) {
        if(currentResult->parent->link[0] == currentResult) {
          currentResult->parent->link[0] = currentResult->link[1];
        }
        else { /* currentResult->parent->link[1] == currentResult*/
          currentResult->parent->link[1] = currentResult->link[1];
        }
      }

      tempResult = currentResult->link[1];

      while(tempResult->link[0]) {
        tempResult = tempResult->link[0];
      }

      tempResult->link[0] = currentResult;
      currentResult->link[1] = tempResult;
    }

    if(currentResult->parent == NULL) {
      break;
    }

    currentResult = currentResult->parent;
  } while(1);

  /* The tree has now been converted to a doubly linked list. We walk thru
  it now to clean up the memory and call the callback */
  currentResult = *root;
  *root = NULL;

  while(currentResult) {
    callback(
      query,
      currentResult->columns,
      i++,
      currentResult
    );

    tempResult = currentResult;
    currentResult = currentResult->link[1];

    free(tempResult);
  }
}
