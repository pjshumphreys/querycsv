void stringGet(
    unsigned char **str,
    struct resultColumnValue *field
) {
  MAC_YIELD

  if(str != NULL) {
    if(*str != NULL) {
      free(*str);
    }

    *str = (unsigned char *)(mystrdup(field->value));
  }
}
