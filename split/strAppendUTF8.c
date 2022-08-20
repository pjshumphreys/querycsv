int strAppendUTF8(QCSV_LONG codepoint, unsigned char **nfdString, size_t *retval) {
  size_t nfdLength = 0;

  MAC_YIELD

  if(retval) {
    nfdLength = *retval;
  }

  if(codepoint < 0x80) {
    reallocMsg((void**)nfdString, nfdLength+1);

    (*nfdString)[nfdLength++] = codepoint;
  }
  else if(codepoint < 0x800) {
    reallocMsg((void**)nfdString, nfdLength+2);

    (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else if(codepoint < 0x10000) {
    reallocMsg((void**)nfdString, nfdLength+3);

    (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else {
    reallocMsg((void**)nfdString, nfdLength+4);

    (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
    (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }

  if(retval) {
    *retval = nfdLength;
  }

  return TRUE;
}
