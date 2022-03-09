int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
  MAC_YIELD

  fprintf(stderr, S_STRING S_STRING, msg, queryData->newLine);

  return 1;
}
