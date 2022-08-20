int compareCodepoints(const void* a, const void* b) __z88dk_params_offset(-4) {
  QCSV_SHORT c = *((QCSV_SHORT*)a);
  QCSV_SHORT d =

  #ifdef __Z88DK
    (*((struct codepointToByte**)b))->codepoint;
  #else
    ((struct codepointToByte*)b)->codepoint;
  #endif

  if(c < d) {
    return -1;
  }

  return (c > d);
}
