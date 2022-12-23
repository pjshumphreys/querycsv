#include <stdio.h>

#define ENC_UNKNOWN 0

int consoleEncoding = ENC_UNKNOWN;
FILE* mystdin = NULL;
FILE* mystdout = NULL;
FILE* mystderr = NULL;

