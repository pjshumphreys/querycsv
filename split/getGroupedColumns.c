void getGroupedColumns(struct qryData *query) {
  struct columnRefHashEntry *currentHashEntry;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;
  int i;

  MAC_YIELD

  /* for each column in the output result set ... */
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        /* ... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way */
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {

          /* convert the aggregation types that need it back into a string */
          switch(currentResultColumn->groupType) {
            case GRP_AVG:
            case GRP_DIS_AVG:
              currentResultColumn->groupNum = fdiv(currentResultColumn->groupNum,
                ctof(currentResultColumn->groupCount));
            case GRP_ROWNUMBER :
            case GRP_SUM:
            case GRP_DIS_SUM:
              ftostr(&(currentResultColumn->groupText), currentResultColumn->groupNum);
            break;
            case GRP_COUNT:
            case GRP_DIS_COUNT:
              d_sprintf(&(currentResultColumn->groupText), D_STRING, currentResultColumn->groupCount);
            break;
            case GRP_STAR:
              d_sprintf(&(currentResultColumn->groupText), D_STRING, query->groupCount);
            break;
          }
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      /* go to the next reference in the hash table */
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
