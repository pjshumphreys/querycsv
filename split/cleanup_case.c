void cleanup_case(struct caseEntry *cases) {
  struct caseEntry *currentCase = cases;

  MAC_YIELD

  while(currentCase) {
    if(currentCase->test) {
      cleanup_expression(currentCase->test);
    }

    cleanup_expression(currentCase->value);
    cases = currentCase;

    currentCase = currentCase->nextInList;

    free(cases);
  }
}
