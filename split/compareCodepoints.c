#include "querycsv.h"

int compareCodepoints(const void* a, const void* b) {
  if((short)(*(long*)a) < ((struct codepointToByte*)b)->codepoint) {
    return -1;
  }

  return (short)(*(long*)a) > ((struct codepointToBytes*)b)->codepoint;
}
