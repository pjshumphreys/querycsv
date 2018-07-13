/* origin: FreeBSD /usr/src/lib/msun/src/e_log10.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/*
 * Return the base 10 logarithm of x.  See log.c for most comments.
 *
 * Reduce x to 2^k (1+f) and calculate r = log(1+f) - f + f*f/2
 * as in log.c, then combine and scale in extra precision:
 *    log10(x) = (f - f*f/2 + r)/log(10) + k*log10(2)
 */

#include "milieu.h"
#include "macros.h"
#include "softfloat.h"

extern double fltMinusOne;
extern double fltZero;
extern double fltOne;
extern double fltFour;
extern double fltTen;
extern double fltHalf;
extern double fltSmall;
extern int fltNotInited;

extern double
two54,
ivln10hi,
ivln10lo,
log10_2hi,
log10_2lo,
Lg1,
Lg2,
Lg3,
Lg4,
Lg5,
Lg6,
Lg7;

double log10a(double x)
{

  double u = x;
  double hfsq, f, s, z, R, w, t1, t2, dk, y, hi, lo, val_hi, val_lo;
  unsigned long hx;
  int k;

  if(fltNotInited) {
    initfp();
  }

  hx = u.high;
  k = 0;

  if(hx < 0x00100000 || hx>>31) {
    if(u.low == 0) {
      return (fdiv(fltMinusOne, fmul(x, x)));  /* log(+-0)=-inf */
    }

    if(hx>>31) {
      return fdiv(fsub(x, x), fltZero); /* log(-#) = NaN */
    }

    /* subnormal number, scale x up */
    k -= 54;
    x = fmul(x, two54);
    u = x;
    hx = u.high;
  }
  else if (hx >= 0x7ff00000) {
    return x;
  }
  else if (hx == 0x3ff00000 && u.low == 0) {
    return fltZero;
  }

  /* reduce x into [sqrt(2)/2, sqrt(2)] */
  hx += 0x3ff00000 - 0x3fe6a09e;
  k += (int)(hx>>20) - 0x3ff;
  hx = (hx&0x000fffff) + 0x3fe6a09e;
  u.high = hx;
  x = u;

  f = fsub(x, fltOne);
  hfsq = fmul(fltHalf, fmul(f, f));
  s = fdiv(f, fadd(ctof(2), f));
  z = fmul(s, s);
  w = fmul(z, z);
  t1 = fmul(w, fadd(Lg2, fmul(w, fadd(Lg4, fmul(w, Lg6)))));
  t2 = fmul(z, fadd(Lg1, fmul(w, fadd(Lg3, fmul(w, fadd(Lg5, fmul(w, Lg7)))))));
  R = fadd(t2, t1);

  /* See log2.c for details. */
  /* hi + lo = f - hfsq + s * (hfsq + R) ~ log(1 + f) */
  hi = fsub(f, hfsq);
  u = hi;
  u.low = 0;
  hi = u;
  lo = fsub(f, fsub(hi, fadd(hfsq, fmul(s, fadd(hfsq, R)))));

  /* val_hi + val_lo ~ log10(1 + f) + k * log10(2) */
  val_hi = fmul(hi, ivln10hi);
  dk = ctof(k);
  y = fmul(dk, log10_2hi);
  val_lo = fadd(fmul(dk, log10_2lo), fadd(fmul(fadd(lo, hi), ivln10lo), fmul(lo, ivln10hi)));

  /*
   * Extra precision in for adding y is not strictly needed
   * since there is no very large cancellation near x = sqrt(2) or
   * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
   * with some parallelism and it reduces the error for many args.
   */
  w = fadd(y, val_hi);
  val_lo = fadd(val_lo, fadd(fsub(y, w), val_hi));
  val_hi = w;

  return fadd(val_lo, val_hi);
}
