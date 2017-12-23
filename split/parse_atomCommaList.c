struct atomEntry *parse_atomCommaList(
    struct qryData *queryData,
    struct atomEntry *lastEntryPtr,
    char *newEntry
) {
  struct atomEntry *newEntryPtr;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&newEntryPtr), sizeof(struct atomEntry));

  if(lastEntryPtr == NULL) {
    newEntryPtr->index = 1;
    newEntryPtr->nextInList = newEntryPtr;
  }
  else {
    newEntryPtr->index = lastEntryPtr->index+1;
    newEntryPtr->nextInList = lastEntryPtr->nextInList;
    lastEntryPtr->nextInList = newEntryPtr;
  }

  newEntryPtr->content = newEntry;

  return newEntryPtr;
}


