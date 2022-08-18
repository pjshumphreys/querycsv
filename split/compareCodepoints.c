int compareCodepoints(const void* a, const void* b) {
  if((unsigned QCSV_SHORT)(*(unsigned QCSV_SHORT*)a) < ((struct codepointToByte*)b)->codepoint) {
    return -1;
  }

  return (unsigned QCSV_SHORT)(*(unsigned QCSV_SHORT*)a) > ((struct codepointToBytes*)b)->codepoint;
}
