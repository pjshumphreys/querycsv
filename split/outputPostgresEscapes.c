void outputPostgresEscapes(char* string, FILE * outputFile, int outputEncoding) {
  char *string2 = string;
  char *string3 = NULL;
  size_t strSize = 0;

  while(*string2) {
    switch(*string2) {
      case ',':
      case '\\':
      case '\r':
      case '\n':
      case '\f':
      case '\t':
      case '\v':
        strAppend('\\', &string3, &strSize);
      break;
    }

    strAppend(*string2, &string3, &strSize);
    string2++;
  }

  strAppend('\0', &string3, &strSize);
  
  fputsEncoded(string3, outputFile, outputEncoding);
  free(string3);
}
