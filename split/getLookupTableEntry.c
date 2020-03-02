static struct hash4Entry numberEntry = { NULL, 127, 0, 0 };

struct hash4Entry * clause4(unsigned char **offset, int totalBytes2) {
  struct hash4Entry *temp;

  temp = in_word_set_a((char *)(*offset), totalBytes2);

  if(temp != NULL) {
    return temp;
  }

  temp = in_word_set_b((char *)(*offset), totalBytes2);

  if(temp != NULL) {
    return temp;
  }

  temp = in_word_set_c((char *)(*offset), totalBytes2);

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

  if(compareNumbers && isNumberWithGetByteLength(*offset, lastMatchedBytes, firstChar)) {
    return &numberEntry;
  }

  totalBytes2 = totalBytes+(*lastMatchedBytes);

  while((temp = clause4(offset, totalBytes2))) {
    /* the match is so far holding up.  */

    /* keep this match for later as it may be the last one we find */
    temp2 = temp;

    /* add the byte length to the total */
    totalBytes += *lastMatchedBytes;

    /* get a code point */
    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)(void)))get))
    (offset, str, totalBytes, lastMatchedBytes, get);

    totalBytes2 = totalBytes+(*lastMatchedBytes);
  }

  /* don't update the value passed to us if we didn't find any match at all */
  if(temp2 != NULL) {
    /* copy the match data into the output */
    *lastMatchedBytes = totalBytes;

    entry->script = temp2->script;
    entry->index = temp2->index;
    entry->isNotLower = temp2->isNotLower;

    return entry;
  }

  return NULL;
}
