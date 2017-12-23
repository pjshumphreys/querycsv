void cleanup_inputTables(struct inputTable *currentInputTable) {
  struct inputTable *next;

  MAC_YIELD

  while(currentInputTable != NULL) {
    next = currentInputTable->nextInputTable;

    free(currentInputTable->queryTableName);
    free(currentInputTable->fileName);
    fclose(currentInputTable->fileStream);
    free(currentInputTable);

    currentInputTable = next;
  }
}
