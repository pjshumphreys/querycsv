int hash_addString(
    struct columnReferenceHash *hashtable,
    char *str,
    struct columnReference *new_list
) {
  struct columnReference *current_list = NULL;
  struct columnRefHashEntry *new_list2 = NULL;
  unsigned int hashval = hash_compare(hashtable, str);

  MAC_YIELD

  /* Does item already exist? */
  current_list = hash_lookupString(hashtable, str);

  /* item already exists, don't insert it again. */
  if(current_list != NULL) {
    return 2;
  }

  reallocMsg((void*)&new_list2, sizeof(struct columnRefHashEntry));

  /* Insert into list */
  new_list2->referenceName = str;
  new_list2->content = new_list;
  new_list2->nextReferenceInHash = hashtable->table[hashval];

  hashtable->table[hashval] = new_list2;

  return 0;
}
