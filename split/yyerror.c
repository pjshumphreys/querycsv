int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
  MAC_YIELD

  fprintf(stderr, "%s\n", msg);

  return 1;
}
