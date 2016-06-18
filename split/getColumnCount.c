#include "querycsv.h"

int getColumnCount(char* inputFileName)
{
  FILE * inputFile = NULL;
  int columnCount = 1;

  //attempt to open the input file
  inputFile = skipBom(inputFileName);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
    columnCount++;
  }

  //output the number of columns we counted
  fprintf(stdout, "%d", columnCount);

  //close the input file and return
  fclose(inputFile);
  return 0;
}
