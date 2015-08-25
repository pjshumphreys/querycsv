//#include "querycsv.h"

struct columnReferenceHash* create_hash_table(int size) {
  struct columnReferenceHash* new_table;

  int i;

  if (size < 1) {
    return NULL; /* invalid size for table */
  }

  /* Attempt to allocate memory for the table structure */
  if ((new_table = malloc(sizeof(struct columnReferenceHash))) == NULL) {
      return NULL;
  }
    
  /* Attempt to allocate memory for the table itself */
  if ((new_table->table = malloc(sizeof(struct columnReference *) * size)) == NULL) {
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

unsigned int hashfunc(struct columnReferenceHash *hashtable, char *str) {
  unsigned int hashval;
  
  /* we start our hash out at 0 */
  hashval = 0;

  /* for each character, we multiply the old hash by 31 and add the current
   * character.  Remember that shifting a number left is equivalent to 
   * multiplying it by 2 raised to the number of places shifted.  So we 
   * are in effect multiplying hashval by 32 and then subtracting hashval.  
   * Why do we do this?  Because shifting and subtraction are much more 
   * efficient operations than multiplication.
   */

  for(; *str != '\0'; str++) {
    hashval = *str + (hashval << 5) - hashval;
  }

  /* we then return the hash value mod the hashtable size so that it will
   * fit into the necessary range
   */

  return hashval % hashtable->size;
}

struct columnReference *lookup_string(struct columnReferenceHash *hashtable, char *str) {
  struct columnRefHashEntry *list;
  unsigned int hashval = hashfunc(hashtable, str);

  /* Go to the correct list based on the hash value and see if str is
   * in the list.  If it is, return return a pointer to the list element.
   * If it isn't, the item isn't in the table, so return NULL.
   */
  for(list = hashtable->table[hashval]; list != NULL; list = list->nextReferenceInHash) {
    if (strcmp(str, list->referenceName) == 0) {
      return list->content;
    }
  }

  return NULL;
}

int add_string(struct columnReferenceHash *hashtable, char *str, struct columnReference *new_list) {
    struct columnReference *current_list;
    struct columnRefHashEntry* new_list2;
    unsigned int hashval = hashfunc(hashtable, str);

    /* Does item already exist? */
    current_list = lookup_string(hashtable, str);

    /* item already exists, don't insert it again. */
    if (current_list != NULL) {
      return 2;
    }

    if ((new_list2 = malloc(sizeof(struct columnRefHashEntry))) == NULL) {
      return 1;
    }
    
    /* Insert into list */
    new_list2->referenceName = str;
    new_list2->content = new_list;
    new_list2->nextReferenceInHash = hashtable->table[hashval];

    
    hashtable->table[hashval] = new_list2;

    return 0;
}

void free_table(struct columnReferenceHash *hashtable) {
  int i;
  struct columnRefHashEntry *list, *temp;

  if (hashtable==NULL) {
    return;
  }

  /* Free the memory for every item in the table, including the 
   * strings themselves.
   */
  for(i=0; i<hashtable->size; i++) {
      list = hashtable->table[i];
      while(list!=NULL) {
          temp = list;
          list = list->nextReferenceInHash;
          free(temp->referenceName);
          free(temp);
      }
  }

  /* Free the table itself */
  free(hashtable->table);
  free(hashtable);
}
