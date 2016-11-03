#include "querycsv.h"

void reallocMsg(char *failureMessage, void **mem, size_t size) {
  void *temp = NULL;

  MAC_YIELD
  
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(failureMessage, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

/* static void* binaryTree;  // if malloc, calloc or realloc fail then try cleaning up some memory by flushing our binary tree to disk */
