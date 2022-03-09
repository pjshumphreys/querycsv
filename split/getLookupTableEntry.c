const struct hash4Entry numberEntry = { NULL, 127, 1, 0 };

struct hash4Entry * clause4(unsigned char **offset, int totalBytes2) {
  struct hash4Entry *temp;

  temp = (struct hash4Entry *)in_word_set_a((char *)(*offset), totalBytes2);

  if(temp != NULL) {
    return temp;
  }

  temp = (struct hash4Entry *)in_word_set_b((char *)(*offset), totalBytes2);

  if(temp != NULL) {
    return temp;
  }

  temp = (struct hash4Entry *)in_word_set_c((char *)(*offset), totalBytes2);

  if(temp != NULL) {
    return temp;
  }

  return NULL;
}

struct hash4Entry *getLookupTableEntry(
    struct hash4Entry *entry,
    unsigned char **offset,
    unsigned char **str,
    int *lastMatchedBytes,
    void (*get)(void),
    int firstChar,
    int compareNumbers
) {
  struct hash4Entry *temp = NULL, *temp2 = NULL;
  int totalBytes = 0;
  int totalBytes2 = 0;

  MAC_YIELD

  if((compareNumbers & 1) && isNumberWithGetByteLength(*offset, lastMatchedBytes, firstChar)) {
    memcpy(entry, &numberEntry, sizeof(struct hash4Entry));

    if(compareNumbers & 2) {
      entry->script = 33;
    }

    return entry;
  }

  totalBytes2 = totalBytes+(*lastMatchedBytes);

  temp = clause4(offset, totalBytes2);

  while(temp != NULL) {
    /* the match is so far holding up.  */

    /* keep this match for later as it may be the last one we find */
    temp2 = temp;

    /* add the byte length to the total */
    totalBytes += *lastMatchedBytes;

    /* get a code point */
    (*((int (*)(unsigned char **, unsigned char **, int,  int *))get))
    (offset, str, totalBytes, lastMatchedBytes);

    totalBytes2 = totalBytes+(*lastMatchedBytes);
    temp = clause4(offset, totalBytes2);
  }

  /* don't update the value passed to us if we didn't find any match at all */
  if(temp2 != NULL) {
    /* copy the match data into the output */
    *lastMatchedBytes = totalBytes;

    entry->script = temp2->script;

    if(compareNumbers & 2 && entry->script == 127) {
      entry->script = 33;
    }

    entry->index = temp2->index;
    entry->isNotLower = temp2->isNotLower;

    return entry;
  }

  return NULL;
}
