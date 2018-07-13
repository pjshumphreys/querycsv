#ifndef MACROS_H
#define MACROS_H

#define IEEE_8087 1
#define NO_LONG_LONG 1

typedef char flag;
typedef struct {
    unsigned int low, high;
} float64;

#define double float64

#define ftostr(_d,_a) { \
  reallocMsg((void**)_d, 32); \
  mydtoa(*(_d), (_a)); \
  reallocMsg((void**)_d, strlen(*(_d)) + 1); \
  } /* mydtoa function should output at
  most 32 characters */
#define fadd(_f,_a) float64_add((_f),(_a))
#define fsub(_f,_a) float64_sub((_f),(_a))
#define fmul(_f,_a) float64_mul((_f),(_a))
#define fdiv(_f,_a) float64_div((_f),(_a))
#define ctof(_s) int32_to_float64(_s)
#define ftoc(_s) float64_to_int32(_s)
#define fneg(_f) float64_mul((_f), fltMinusOne)
#define isnan(_f) (float64_is_nan(_f))
#define isinf(_f) (!finitea(_f))
#define floor(_f) float64_round_to_int(_f)

#define feq(_d,_s) float64_eq((_d),(_s))
#define fcmp(_d,_s) (!float64_eq((_d),(_s)))
#define fgt(_d,_s) (!float64_le((_d),(_s)))

void initfp(void);
double pow10a(int exp);
double log10a(double x);
int finitea(double x);
flag float64_is_nan(double a);

#endif
