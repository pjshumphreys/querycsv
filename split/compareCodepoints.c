int compareCodepoints(const void* a, const void* b) {
  if((QCSV_SHORT)(*(QCSV_SHORT*)a) < ((struct codepointToByte*)b)->codepoint) {
    return -1;
  }

  return (QCSV_SHORT)(*(QCSV_SHORT*)a) > ((struct codepointToBytes*)b)->codepoint;
}
