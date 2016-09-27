#include "querycsv.h"

int tree_insert(
    struct qryData *query,
    struct resultColumnValue *columns,
    struct resultTree **root
  ) {

  struct resultTree *currentResult;
  struct resultTree *newResult;
  int comparison;

  MAC_YIELD

  if(root == NULL) {
    //Huh? no memory location was specified to hold the tree?
    //Just exit and let the author of the calling function figure out their mistake
    return FALSE;
  }

  if((newResult = malloc(sizeof(struct resultTree))) == NULL) {
    return FALSE;
  }

  newResult->parent = NULL;
  newResult->left = NULL;
  newResult->right = NULL;
  newResult->columns = columns;
  newResult->type = TRE_BLACK;

  if(*root == NULL) {
    //No entries have been inserted at all.
    //Just insert the data into a new node
    *root = newResult;
    return TRUE;
  }
  else {
    //navigate down the tree, and insert the new data into the correct place within it

    //start at the top
    currentResult = *root;

    for( ; ; ) {
      comparison = recordCompare(currentResult->columns, columns, query);

      if(comparison < 1) {
        if(currentResult->left == NULL) {
          newResult->parent = currentResult;
          currentResult->left = newResult;
          return TRUE;
        }
        else {
          currentResult = currentResult->left;
        }
      }
      else {
        if(currentResult->right == NULL) {
          newResult->parent = currentResult;
          currentResult->right = newResult;
          return TRUE;
        }
        else {
          currentResult = currentResult->right;
        }
      }
    }
  }
}
