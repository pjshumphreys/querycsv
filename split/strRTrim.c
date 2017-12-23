/* decrement size of a string to represent right trimming whitespace  */
int strRTrim(char **value, size_t *strSize, char *minSize) {
  char *end;
  char *str;
  int size;

  MAC_YIELD

  if(value == NULL || strSize == NULL || *value == NULL) {
    return FALSE;
  }

  str = *value;
  size = *strSize;

  end = str + size - 1;
  while(end > str && end != minSize && (*end == ' ' || *end == '\t')) {
    end--;
    size--;
  }

  *strSize = size;

  return TRUE;
}
