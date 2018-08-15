void reallocMsg(void **mem, size_t size) {
  void *temp;

  MAC_YIELD

  if(mem != NULL) {
    if(size) {
      temp = realloc(*mem, size);

      if(temp == NULL) {
        fwrite(TDB_MALLOC_FAILED3, stderr);
        exit(EXIT_FAILURE);
      }

      *mem = temp;
    }
    else {
      free(*mem);
      *mem = NULL;
    }
  }
  else {
    fputs(TDB_INVALID_REALLOC, stderr);
    exit(EXIT_FAILURE);
  }
}

/* static void* binaryTree;  // if malloc, calloc or realloc fail then try cleaning up some memory by flushing our binary tree to disk */
