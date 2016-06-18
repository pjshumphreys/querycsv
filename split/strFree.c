#include "querycsv.h"

void strFree(char ** str)
//free memory and set the pointer to NULL. this function should hopefully get inlined
{
  free(*str);
  *str = NULL;
}
