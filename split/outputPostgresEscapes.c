void outputPostgresEscapes(char* string, struct qryData *query) {
  char *string2 = string;
  char *string3 = NULL;
  size_t strSize = 0;

  while(*string2) {
    switch(*string2) {
      case ',':
        strAppend('\\', &string3, &strSize);
        strAppend(',', &string3, &strSize);
      break;

      case '\\':
        strAppend('\\', &string3, &strSize);
        strAppend('\\', &string3, &strSize);
      break;

      case '\r':
        strAppend('\\', &string3, &strSize);
        strAppend('r', &string3, &strSize);
      break;

      case '\n':
        strAppend('\\', &string3, &strSize);
        strAppend('n', &string3, &strSize);
      break;

      case '\f':
        strAppend('\\', &string3, &strSize);
        strAppend('f', &string3, &strSize);
      break;

      case '\t':
        strAppend('\\', &string3, &strSize);
        strAppend('t', &string3, &strSize);
      break;

      case '\v':
        strAppend('\\', &string3, &strSize);
        strAppend('v', &string3, &strSize);
      break;

      default:
        strAppend(*string2, &string3, &strSize);
      break;
    }

    string2++;
  }

  strAppend('\0', &string3, &strSize);

  fputsEncoded(string3, query);
  free(string3);
}
