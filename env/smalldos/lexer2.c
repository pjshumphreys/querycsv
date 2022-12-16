#pragma inline_ depth (0)
#define YY_USE_CONST 1
#define YY_BUF_SIZE 512

#include "build/lexer2.h"

flex_int8_t yy_accept2(unsigned int offset) {
  return yy_accept[offset];
}

flex_uint16_t yy_nxt2(unsigned int offset) {
  return yy_nxt[offset];
}

flex_int16_t yy_chk2(unsigned int offset) {
  return yy_chk[offset];
}

YY_CHAR yy_ec2(unsigned int offset) {
  return yy_ec[offset];
}
