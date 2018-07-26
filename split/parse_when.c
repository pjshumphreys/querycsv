struct caseEntry *parse_when(
    struct qryData *queryData,
    struct caseEntry *initialEntries,
    struct expression* newTest,
    struct expression* newResult
) {
  struct caseEntry* newEntry = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void **)&newEntry, sizeof(struct caseEntry));

  newEntry->test = newTest;
  newEntry->value = newResult;

  if(initialEntries == NULL) {
    newEntry->nextInList = newEntry;
  }
  else {
    newEntry->nextInList = initialEntries->nextInList;
    initialEntries->nextInList = newEntry;
  }

  return newEntry;
}

