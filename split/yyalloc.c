void * yyalloc (size_t bytes, void* yyscanner) {
  void *temp;

  if((temp = malloc(bytes)) == NULL) {
    exit(EXIT_FAILURE);
  }

  return temp;
}

void * yyrealloc (void * ptr, size_t bytes, void* yyscanner) {
  void *temp;

  if((temp = realloc(ptr, bytes)) == NULL) {
    exit(EXIT_FAILURE);
  }

  return temp;
}

void * yymalloc (size_t bytes) {
  void *temp;

  if((temp = malloc(bytes)) == NULL) {
    exit(EXIT_FAILURE);
  }

  return temp;
}
