void cleanup_columnReferences(struct columnReferenceHash *table) {
  int i;
  struct columnRefHashEntry *currentHashEntry, *nextHashEntry;
  struct columnReference *currentReference, *nextReference;

  MAC_YIELD

  for(i = 0; i < table->size; i++) {
    currentHashEntry = table->table[i];

    while(currentHashEntry != NULL) {
      nextHashEntry = currentHashEntry->nextReferenceInHash;

      free(currentHashEntry->referenceName);
      currentReference = currentHashEntry->content;
      free(currentHashEntry);

      while(currentReference != NULL) {
        nextReference = currentReference->nextReferenceWithName;

        if(currentReference->referenceType == REF_COLUMN) {
          free(currentReference->reference.columnPtr);
        }

        if(currentReference->referenceType == REF_EXPRESSION) {
          cleanup_expression(currentReference->reference.calculatedPtr.expressionPtr);
        }

        free(currentReference);

        currentReference = nextReference;
      }

      currentHashEntry = nextHashEntry;
    }
  }

  free(table->table);
  free(table);
}
