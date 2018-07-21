int needsEscaping(char *str, int params) {
  MAC_YIELD

  if(
      /* empty string always needs escaping */
      *str == '\0' ||

      /* always escape the string "\N" to work well with systems that store nulls in csv files*/
      strcmp(str, "\\N") == 0 ||
      strcmp(str, "NULL") == 0
    ) {
    return TRUE;
  }

  /* test for leading whitespace */
  switch(*str) {
    case ' ':
    case '\t':
      return TRUE;
  }

  while(*str) {
    switch(*str) {
      case '"':
      case '\r':
      case '\n':
      case ',':
      case ';': /* used as a field delimiter by european dsv files, so also needs escaping */
        return TRUE;

      case '\302': {
        str++;

        if(*str == '\205') {/* EBCDIC newline */
          return TRUE;
        }

        str--;
      }
    }

    str++;
  }

  /* test for trailing whitespace */
  str--;
  switch(*str) {
    case ' ':
    case '\t':
      return TRUE;
  }

  return FALSE;
}
