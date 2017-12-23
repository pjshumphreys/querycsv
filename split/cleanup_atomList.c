void cleanup_atomList(struct atomEntry *currentAtom) {
  struct atomEntry* next;

  MAC_YIELD

  while(currentAtom != NULL) {
    next = currentAtom->nextInList;

    free(currentAtom->content);
    free(currentAtom);

    currentAtom = next;
  }
}
