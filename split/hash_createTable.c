struct columnReferenceHash *hash_createTable(int size) {
  struct columnReferenceHash *new_table = NULL;
  struct columnRefHashEntry **theTable = NULL;
  int i;

  MAC_YIELD

  if(size < 1) {
    return NULL; /* invalid size for table */
  }

  /* Allocate memory for the table structure */
  reallocMsg((void**)&new_table, sizeof(struct columnReferenceHash));

  /* Allocate memory for the table itself */
  reallocMsg((void**)&theTable, sizeof(struct columnRefHashEntry *) * size);

  new_table->table = theTable;

  /* Initialize the elements of the table */
  for(i=0; i<size; i++) {
    new_table->table[i] = NULL;
  }

  /* Set the table's size */
  new_table->size = size;

  return new_table;
}
