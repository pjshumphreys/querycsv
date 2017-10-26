#include "querycsv.h"

struct columnReferenceHash *hash_createTable(int size) {
  struct columnReferenceHash *new_table;

  int i;

  MAC_YIELD

  if(size < 1) {
    return NULL; /* invalid size for table */
  }

  /* Attempt to allocate memory for the table structure */
  if((new_table = malloc(sizeof(struct columnReferenceHash))) == NULL) {
    return NULL;
  }

  /* Attempt to allocate memory for the table itself */
  if((new_table->table = malloc(sizeof(struct columnReference *) * size)) == NULL) {
    free(new_table);
    return NULL;
  }

  /* Initialize the elements of the table */
  for(i=0; i<size; i++) {
    new_table->table[i] = NULL;
  }

  /* Set the table's size */
  new_table->size = size;

  return new_table;
}
