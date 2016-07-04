#include "querycsv.h"

//Calls a callback function once for every item in the tree, and de-allocates memory used by the tree in the process

//This function only de-allocates/frees memory for the tree, ***but not the data items the tree contains***
void tree_walkAndCleanup(
    struct qryData *query,
    struct resultTree **root,
    void (*callback)(struct qryData *, struct resultColumnValue *, int)
  ) {
  struct resultTree *currentResult;
  struct resultTree *parentResult;
  int i = 0;
  
  if(root == NULL || *root == NULL) {
    return;
  }

  currentResult = *root;

  for( ; ; ) {
    if(currentResult->left) {
      currentResult = currentResult->left;
      continue;
    }

    if((currentResult->type) != TRE_FREED) {
      callback(
        query,
        currentResult->columns,
        i
      );

      currentResult->type = TRE_FREED;
      i+=1;
    }

    if(currentResult->right) {
      currentResult = currentResult->right;
      continue;
    }

    parentResult = currentResult->parent;
    
    if(parentResult != NULL) {
      if(parentResult->left == currentResult) {
        currentResult = parentResult;
        free(currentResult->left);
        currentResult->left = NULL;
      }
      else {
        currentResult = parentResult;
        free(currentResult->right);
        currentResult->right = NULL;
      }
    }
    else {
      free(currentResult);
      *root = NULL;
      return;
    }
  }
}
