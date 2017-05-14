#define YY_INPUT(buff,result,max_size) { \
  char* buf = buff; \
  long c = 0x0065; \
  int i; \
  size_t n = 0; \
  long codepointBuffer[4]; \
  int arrLength; \
  int byteLength; \
  while (n < max_size - 16 && c != MYEOF) { \
    yyextra->getCodepoints(yyin, \
      codepointBuffer, \
      &arrLength, \
      &byteLength \
    );\
    for (i = 0; i < arrLength; i++) { \
      c = codepointBuffer[i]; \
      if(c == MYEOF) { \
        break; \
      } \
      if (c < 0x80) { \
        buf[n++] = (char)c; \
      } \
      else if (c < 0x800) { \
        buf[n++] = ((c >> 6) + 0xC0); \
        buf[n++] = ((c & 0x3F) + 0x80); \
      } \
      else if (c < 0x10000) { \
        buf[n++] = ((c >> 12) + 0xE0); \
        buf[n++] = (((c >> 6) & 0x3F) + 0x80); \
        buf[n++] = ((c & 0x3F) + 0x80); \
      } \
      else { \
        buf[n++] = ((c >> 18) + 0xF0); \
        buf[n++] = (((c >> 12) & 0x3F) + 0x80); \
        buf[n++] = (((c >> 6) & 0x3F) + 0x80); \
        buf[n++] = ((c & 0x3F) + 0x80); \
      } \
    } \
  } \
  if (c == MYEOF && ferror(yyin)) { \
    YY_FATAL_ERROR("input in flex scanner failed"); \
  } \
  result = n; \
}
