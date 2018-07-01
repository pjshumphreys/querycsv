int compareCodepoints(const void* a, const void* b) {
  if((QCSV_SHORT)(*(long*)a) < ((struct codepointToByte*)b)->codepoint) {
    return -1;
  }

  return (QCSV_SHORT)(*(long*)a) > ((struct codepointToBytes*)b)->codepoint;
}
