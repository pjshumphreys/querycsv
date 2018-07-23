struct expression *parse_case(
    struct qryData *queryData,
    struct expression* simpleCompareTo,
    struct caseEntry* cases,
    struct expression* elseValue
) {

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  return parse_scalarExpLiteral(queryData, "");
}

