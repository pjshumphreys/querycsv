#include "querycsv.h"

int combiningCharCompare(const void* a, const void* b)
{
  return (*((long*)a) < *((long*)b)) ? -1 :
		(*((long*)a) != *((long*)b) ? 1 : 0);
}
