#define YY_USE_CONST 1

#include "lexer2.h"

flex_int8_t yy_accept2(unsigned int offset) {
  return yy_accept[offset];
}

flex_uint16_t yy_nxt2(unsigned int offset) {
  return yy_nxt[offset];
}

flex_int16_t yy_chk2(unsigned int offset) {
  return yy_chk[offset];
}
