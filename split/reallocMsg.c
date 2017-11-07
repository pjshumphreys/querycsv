#include "querycsv.h"

void reallocMsg(void **mem, size_t size) {
  void *temp;

  MAC_YIELD

  if(mem != NULL) {
    temp = realloc(*mem, size);

    if(temp == NULL) {
      fputs(TDB_MALLOC_FAILED, stderr);
      exit(EXIT_FAILURE);
    }

    *mem = temp;
  }
  else {
    fputs(TDB_INVALID_REALLOC, stderr);
  }
}

/* static void* binaryTree;  // if malloc, calloc or realloc fail then try cleaning up some memory by flushing our binary tree to disk */
