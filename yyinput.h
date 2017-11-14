#define YY_INPUT(buf,result,max_size) { \
  myyyinput(yyin, yyextra, buf, &(result), max_size); \
}
