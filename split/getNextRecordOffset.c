#include "querycsv.h"

int getNextRecordOffset(char *inputFileName, long offset) {
  FILE *inputFile = NULL;

  MAC_YIELD

  //attempt to open the input file
  inputFile = skipBom(inputFileName);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  //seek to offset
  if(fseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
    //do nothing
  }

  //get current file position
  fprintf(stdout, "%ld", ftell(inputFile));

  //close the input file and return
  fclose(inputFile);
  return 0;
}
