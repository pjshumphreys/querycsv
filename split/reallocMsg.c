#include "querycsv.h"

void reallocMsg(void **mem, size_t size) {
  void *temp = NULL;

  MAC_YIELD

  if(mem != NULL) {
    if(
        (*mem == NULL && (temp = calloc(1, size)) == NULL) ||
        (temp = realloc(*mem, size)) == NULL
    ) {
      fputs(TDB_MALLOC_FAILED, stderr);
      exit(EXIT_FAILURE);
    }

    *mem = temp;
  }
}

/* static void* binaryTree;  // if malloc, calloc or realloc fail then try cleaning up some memory by flushing our binary tree to disk */
