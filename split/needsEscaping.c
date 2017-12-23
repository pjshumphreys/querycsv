int needsEscaping(char *str, int params) {
  MAC_YIELD

  if(
    str == NULL ||
    strcmp(str, (((params & PRM_EXPORT) == 0)?"\\N":"\\N")) == 0 ||
    *str == ' ' ||
    *str == '\t') {
    return TRUE;
  }

  while(*str) {
    if(*str == '"' || *str == '\n' || *str == ',') {
    return TRUE;
    }
    str++;
  }

  str--;

  if(*str == ' ' || *str == '\t') {
    return TRUE;
  }

  return FALSE;
}
