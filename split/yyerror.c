#include "querycsv.h"

int yyerror(struct qryData* queryData, void* scanner, const char *msg) {
    fprintf(stderr, msg);
    return 1;
}
