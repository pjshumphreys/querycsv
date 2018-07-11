
/*============================================================================

This C source file is part of the Berkeley SoftFloat IEEE Floating-Point
Arithmetic Package, Release 2c, by John R. Hauser.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TOLERATE ALL LOSSES, COSTS, OR OTHER
PROBLEMS THEY INCUR DUE TO THE SOFTWARE WITHOUT RECOMPENSE FROM JOHN HAUSER OR
THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE, AND WHO FURTHERMORE EFFECTIVELY
INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE
(possibly via similar legal notice) AGAINST ALL LOSSES, COSTS, OR OTHER
PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE, OR
INCURRED BY ANYONE DUE TO A DERIVATIVE WORK THEY CREATE USING ANY PART OF THE
SOFTWARE.

Derivative works require also that (1) the source code for the derivative work
includes prominent notice that the work is derivative, and (2) the source code
includes prominent notice of these three paragraphs for those parts of this
code that are retained.

=============================================================================*/

#include "macros.h"
#include "milieu.h"
#include "softfloat.h"
#include <stdio.h>

/*----------------------------------------------------------------------------
| Floating-point rounding mode and exception flags.
*----------------------------------------------------------------------------*/
int8 float_rounding_mode = float_round_down;
int8 float_exception_flags = 0;

/*----------------------------------------------------------------------------
| Primitive arithmetic functions, including multi-word arithmetic, and
| division and square root approximations.  (Can be specialized to target if
| desired.)
*----------------------------------------------------------------------------*/

/*============================================================================

This C source fragment is part of the Berkeley SoftFloat IEEE Floating-Point
Arithmetic Package, Release 2c, by John R. Hauser.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TOLERATE ALL LOSSES, COSTS, OR OTHER
PROBLEMS THEY INCUR DUE TO THE SOFTWARE WITHOUT RECOMPENSE FROM JOHN HAUSER OR
THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE, AND WHO FURTHERMORE EFFECTIVELY
INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE
(possibly via similar legal notice) AGAINST ALL LOSSES, COSTS, OR OTHER
PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE, OR
INCURRED BY ANYONE DUE TO A DERIVATIVE WORK THEY CREATE USING ANY PART OF THE
SOFTWARE.

Derivative works require also that (1) the source code for the derivative work
includes prominent notice that the work is derivative, and (2) the source code
includes prominent notice of these three paragraphs for those parts of this
code that are retained.

=============================================================================*/

/*----------------------------------------------------------------------------
| Shifts `a' right by the number of bits given in `count'.  If any nonzero
| bits are shifted off, they are "jammed" into the least significant bit of
| the result by setting the least significant bit to 1.  The value of `count'
| can be arbitrarily large; in particular, if `count' is greater than 32, the
| result will be either 0 or 1, depending on whether `a' is zero or nonzero.
| The result is stored in the location pointed to by `zPtr'.
*----------------------------------------------------------------------------*/

INLINE void shift32RightJamming( bits32 a, int16 count, bits32 *zPtr )
{
    bits32 z;

    if ( count == 0 ) {
        z = a;
    }
    else if ( count < 32 ) {
        z = ( a>>count ) | ( ( a<<( ( - count ) & 31 ) ) != 0 );
    }
    else {
        z = ( a != 0 );
    }
    *zPtr = z;

}

/*----------------------------------------------------------------------------
| Shifts the 64-bit value formed by concatenating `a0' and `a1' right by the
| number of bits given in `count'.  Any bits shifted off are lost.  The value
| of `count' can be arbitrarily large; in particular, if `count' is greater
| than 64, the result will be 0.  The result is broken into two 32-bit pieces
| which are stored at the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 shift64Right(
     bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr )
{
    bits32 z0, z1;
    int8 negCount = ( - count ) & 31;

    if ( count == 0 ) {
        z1 = a1;
        z0 = a0;
    }
    else if ( count < 32 ) {
        z1 = ( a0<<negCount ) | ( a1>>count );
        z0 = a0>>count;
    }
    else {
        z1 = ( count < 64 ) ? ( a0>>( count & 31 ) ) : 0;
        z0 = 0;
    }
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Shifts the 64-bit value formed by concatenating `a0' and `a1' right by the
| number of bits given in `count'.  If any nonzero bits are shifted off, they
| are "jammed" into the least significant bit of the result by setting the
| least significant bit to 1.  The value of `count' can be arbitrarily large;
| in particular, if `count' is greater than 64, the result will be either 0
| or 1, depending on whether the concatenation of `a0' and `a1' is zero or
| nonzero.  The result is broken into two 32-bit pieces which are stored at
| the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 shift64RightJamming(
     bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr )
{
    bits32 z0, z1;
    int8 negCount = ( - count ) & 31;

    if ( count == 0 ) {
        z1 = a1;
        z0 = a0;
    }
    else if ( count < 32 ) {
        z1 = ( a0<<negCount ) | ( a1>>count ) | ( ( a1<<negCount ) != 0 );
        z0 = a0>>count;
    }
    else {
        if ( count == 32 ) {
            z1 = a0 | ( a1 != 0 );
        }
        else if ( count < 64 ) {
            z1 = ( a0>>( count & 31 ) ) | ( ( ( a0<<negCount ) | a1 ) != 0 );
        }
        else {
            z1 = ( ( a0 | a1 ) != 0 );
        }
        z0 = 0;
    }
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Shifts the 96-bit value formed by concatenating `a0', `a1', and `a2' right
| by 32 _plus_ the number of bits given in `count'.  The shifted result is
| at most 64 nonzero bits; these are broken into two 32-bit pieces which are
| stored at the locations pointed to by `z0Ptr' and `z1Ptr'.  The bits shifted
| off form a third 32-bit result as follows:  The _last_ bit shifted off is
| the most-significant bit of the extra result, and the other 31 bits of the
| extra result are all zero if and only if _all_but_the_last_ bits shifted off
| were all zero.  This extra result is stored in the location pointed to by
| `z2Ptr'.  The value of `count' can be arbitrarily large.
|     (This routine makes more sense if `a0', `a1', and `a2' are considered
| to form a fixed-point value with binary point between `a1' and `a2'.  This
| fixed-point value is shifted right by the number of bits given in `count',
| and the integer part of the result is returned at the locations pointed to
| by `z0Ptr' and `z1Ptr'.  The fractional part of the result may be slightly
| corrupted as described above, and is returned at the location pointed to by
| `z2Ptr'.)
*----------------------------------------------------------------------------*/

INLINE void
 shift64ExtraRightJamming(
     bits32 a0,
     bits32 a1,
     bits32 a2,
     int16 count,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr
 )
{
    bits32 z0, z1, z2;
    int8 negCount = ( - count ) & 31;

    if ( count == 0 ) {
        z2 = a2;
        z1 = a1;
        z0 = a0;
    }
    else {
        if ( count < 32 ) {
            z2 = a1<<negCount;
            z1 = ( a0<<negCount ) | ( a1>>count );
            z0 = a0>>count;
        }
        else {
            if ( count == 32 ) {
                z2 = a1;
                z1 = a0;
            }
            else {
                a2 |= a1;
                if ( count < 64 ) {
                    z2 = a0<<negCount;
                    z1 = a0>>( count & 31 );
                }
                else {
                    z2 = ( count == 64 ) ? a0 : ( a0 != 0 );
                    z1 = 0;
                }
            }
            z0 = 0;
        }
        z2 |= ( a2 != 0 );
    }
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Shifts the 64-bit value formed by concatenating `a0' and `a1' left by the
| number of bits given in `count'.  Any bits shifted off are lost.  The value
| of `count' must be less than 32.  The result is broken into two 32-bit
| pieces which are stored at the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 shortShift64Left(
     bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr )
{

    *z1Ptr = a1<<count;
    *z0Ptr =
        ( count == 0 ) ? a0 : ( a0<<count ) | ( a1>>( ( - count ) & 31 ) );

}

/*----------------------------------------------------------------------------
| Shifts the 96-bit value formed by concatenating `a0', `a1', and `a2' left
| by the number of bits given in `count'.  Any bits shifted off are lost.
| The value of `count' must be less than 32.  The result is broken into three
| 32-bit pieces which are stored at the locations pointed to by `z0Ptr',
| `z1Ptr', and `z2Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 shortShift96Left(
     bits32 a0,
     bits32 a1,
     bits32 a2,
     int16 count,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr
 )
{
    bits32 z0, z1, z2;
    int8 negCount;

    z2 = a2<<count;
    z1 = a1<<count;
    z0 = a0<<count;
    if ( 0 < count ) {
        negCount = ( ( - count ) & 31 );
        z1 |= a2>>negCount;
        z0 |= a1>>negCount;
    }
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Adds the 64-bit value formed by concatenating `a0' and `a1' to the 64-bit
| value formed by concatenating `b0' and `b1'.  Addition is modulo 2^64, so
| any carry out is lost.  The result is broken into two 32-bit pieces which
| are stored at the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 add64(
     bits32 a0, bits32 a1, bits32 b0, bits32 b1, bits32 *z0Ptr, bits32 *z1Ptr )
{
    bits32 z1;

    z1 = a1 + b1;
    *z1Ptr = z1;
    *z0Ptr = a0 + b0 + ( z1 < a1 );

}

/*----------------------------------------------------------------------------
| Adds the 96-bit value formed by concatenating `a0', `a1', and `a2' to the
| 96-bit value formed by concatenating `b0', `b1', and `b2'.  Addition is
| modulo 2^96, so any carry out is lost.  The result is broken into three
| 32-bit pieces which are stored at the locations pointed to by `z0Ptr',
| `z1Ptr', and `z2Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 add96(
     bits32 a0,
     bits32 a1,
     bits32 a2,
     bits32 b0,
     bits32 b1,
     bits32 b2,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr
 )
{
    bits32 z0, z1, z2;
    int8 carry0, carry1;

    z2 = a2 + b2;
    carry1 = ( z2 < a2 );
    z1 = a1 + b1;
    carry0 = ( z1 < a1 );
    z0 = a0 + b0;
    z1 += carry1;
    z0 += ( z1 < carry1 );
    z0 += carry0;
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Subtracts the 64-bit value formed by concatenating `b0' and `b1' from the
| 64-bit value formed by concatenating `a0' and `a1'.  Subtraction is modulo
| 2^64, so any borrow out (carry out) is lost.  The result is broken into two
| 32-bit pieces which are stored at the locations pointed to by `z0Ptr' and
| `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 sub64(
     bits32 a0, bits32 a1, bits32 b0, bits32 b1, bits32 *z0Ptr, bits32 *z1Ptr )
{

    *z1Ptr = a1 - b1;
    *z0Ptr = a0 - b0 - ( a1 < b1 );

}

/*----------------------------------------------------------------------------
| Subtracts the 96-bit value formed by concatenating `b0', `b1', and `b2' from
| the 96-bit value formed by concatenating `a0', `a1', and `a2'.  Subtraction
| is modulo 2^96, so any borrow out (carry out) is lost.  The result is broken
| into three 32-bit pieces which are stored at the locations pointed to by
| `z0Ptr', `z1Ptr', and `z2Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 sub96(
     bits32 a0,
     bits32 a1,
     bits32 a2,
     bits32 b0,
     bits32 b1,
     bits32 b2,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr
 )
{
    bits32 z0, z1, z2;
    int8 borrow0, borrow1;

    z2 = a2 - b2;
    borrow1 = ( a2 < b2 );
    z1 = a1 - b1;
    borrow0 = ( a1 < b1 );
    z0 = a0 - b0;
    z0 -= ( z1 < borrow1 );
    z1 -= borrow1;
    z0 -= borrow0;
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Multiplies `a' by `b' to obtain a 64-bit product.  The product is broken
| into two 32-bit pieces which are stored at the locations pointed to by
| `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

INLINE void mul32To64( bits32 a, bits32 b, bits32 *z0Ptr, bits32 *z1Ptr )
{
    bits16 aHigh, aLow, bHigh, bLow;
    bits32 z0, zMiddleA, zMiddleB, z1;

    aLow = a;
    aHigh = a>>16;
    bLow = b;
    bHigh = b>>16;
    z1 = ( (bits32) aLow ) * bLow;
    zMiddleA = ( (bits32) aLow ) * bHigh;
    zMiddleB = ( (bits32) aHigh ) * bLow;
    z0 = ( (bits32) aHigh ) * bHigh;
    zMiddleA += zMiddleB;
    z0 += ( ( (bits32) ( zMiddleA < zMiddleB ) )<<16 ) + ( zMiddleA>>16 );
    zMiddleA <<= 16;
    z1 += zMiddleA;
    z0 += ( z1 < zMiddleA );
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Multiplies the 64-bit value formed by concatenating `a0' and `a1' by `b'
| to obtain a 96-bit product.  The product is broken into three 32-bit pieces
| which are stored at the locations pointed to by `z0Ptr', `z1Ptr', and
| `z2Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 mul64By32To96(
     bits32 a0,
     bits32 a1,
     bits32 b,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr
 )
{
    bits32 z0, z1, z2, more1;

    mul32To64( a1, b, &z1, &z2 );
    mul32To64( a0, b, &z0, &more1 );
    add64( z0, more1, 0, z1, &z0, &z1 );
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Multiplies the 64-bit value formed by concatenating `a0' and `a1' to the
| 64-bit value formed by concatenating `b0' and `b1' to obtain a 128-bit
| product.  The product is broken into four 32-bit pieces which are stored at
| the locations pointed to by `z0Ptr', `z1Ptr', `z2Ptr', and `z3Ptr'.
*----------------------------------------------------------------------------*/

INLINE void
 mul64To128(
     bits32 a0,
     bits32 a1,
     bits32 b0,
     bits32 b1,
     bits32 *z0Ptr,
     bits32 *z1Ptr,
     bits32 *z2Ptr,
     bits32 *z3Ptr
 )
{
    bits32 z0, z1, z2, z3;
    bits32 more1, more2;

    mul32To64( a1, b1, &z2, &z3 );
    mul32To64( a1, b0, &z1, &more2 );
    add64( z1, more2, 0, z2, &z1, &z2 );
    mul32To64( a0, b0, &z0, &more1 );
    add64( z0, more1, 0, z1, &z0, &z1 );
    mul32To64( a0, b1, &more1, &more2 );
    add64( more1, more2, 0, z2, &more1, &z2 );
    add64( z0, z1, 0, more1, &z0, &z1 );
    *z3Ptr = z3;
    *z2Ptr = z2;
    *z1Ptr = z1;
    *z0Ptr = z0;

}

/*----------------------------------------------------------------------------
| Returns an approximation to the 32-bit integer quotient obtained by dividing
| `b' into the 64-bit value formed by concatenating `a0' and `a1'.  The
| divisor `b' must be at least 2^31.  If q is the exact quotient truncated
| toward zero, the approximation returned lies between q and q + 2 inclusive.
| If the exact quotient q is larger than 32 bits, the maximum positive 32-bit
| unsigned integer is returned.
*----------------------------------------------------------------------------*/

static bits32 estimateDiv64To32( bits32 a0, bits32 a1, bits32 b )
{
    bits32 b0, b1;
    bits32 rem0, rem1, term0, term1;
    bits32 z;

    if ( b <= a0 ) return 0xFFFFFFFF;
    b0 = b>>16;
    z = ( b0<<16 <= a0 ) ? 0xFFFF0000 : ( a0 / b0 )<<16;
    mul32To64( b, z, &term0, &term1 );
    sub64( a0, a1, term0, term1, &rem0, &rem1 );
    while ( ( (sbits32) rem0 ) < 0 ) {
        z -= 0x10000;
        b1 = b<<16;
        add64( rem0, rem1, b0, b1, &rem0, &rem1 );
    }
    rem0 = ( rem0<<16 ) | ( rem1>>16 );
    z |= ( b0<<16 <= rem0 ) ? 0xFFFF : rem0 / b0;
    return z;

}

/*----------------------------------------------------------------------------
| Returns an approximation to the square root of the 32-bit significand given
| by `a'.  Considered as an integer, `a' must be at least 2^31.  If bit 0 of
| `aExp' (the least significant bit) is 1, the integer returned approximates
| 2^31*sqrt(`a'/2^31), where `a' is considered an integer.  If bit 0 of `aExp'
| is 0, the integer returned approximates 2^31*sqrt(`a'/2^30).  In either
| case, the approximation returned lies strictly within +/-2 of the exact
| value.
*----------------------------------------------------------------------------*/

static bits32 estimateSqrt32( int16 aExp, bits32 a )
{
    static const bits16 sqrtOddAdjustments[] = {
        0x0004, 0x0022, 0x005D, 0x00B1, 0x011D, 0x019F, 0x0236, 0x02E0,
        0x039C, 0x0468, 0x0545, 0x0631, 0x072B, 0x0832, 0x0946, 0x0A67
    };
    static const bits16 sqrtEvenAdjustments[] = {
        0x0A2D, 0x08AF, 0x075A, 0x0629, 0x051A, 0x0429, 0x0356, 0x029E,
        0x0200, 0x0179, 0x0109, 0x00AF, 0x0068, 0x0034, 0x0012, 0x0002
    };
    int8 index;
    bits32 z;

    index = ( a>>27 ) & 15;
    if ( aExp & 1 ) {
        z = 0x4000 + ( a>>17 ) - sqrtOddAdjustments[ index ];
        z = ( ( a / z )<<14 ) + ( z<<15 );
        a >>= 1;
    }
    else {
        z = 0x8000 + ( a>>17 ) - sqrtEvenAdjustments[ index ];
        z = a / z + z;
        z = ( 0x20000 <= z ) ? 0xFFFF8000 : ( z<<15 );
        if ( z <= a ) return (bits32) ( ( (sbits32) a )>>1 );
    }
    return ( ( estimateDiv64To32( a, 0, z ) )>>1 ) + ( z>>1 );

}

/*----------------------------------------------------------------------------
| Returns the number of leading 0 bits before the most-significant 1 bit of
| `a'.  If `a' is zero, 32 is returned.
*----------------------------------------------------------------------------*/

static int8 countLeadingZeros32( bits32 a )
{
    static const int8 countLeadingZerosHigh[] = {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int8 shiftCount;

    shiftCount = 0;
    if ( a < 0x10000 ) {
        shiftCount += 16;
        a <<= 16;
    }
    if ( a < 0x1000000 ) {
        shiftCount += 8;
        a <<= 8;
    }
    shiftCount += countLeadingZerosHigh[ a>>24 ];
    return shiftCount;

}

/*----------------------------------------------------------------------------
| Returns 1 if the 64-bit value formed by concatenating `a0' and `a1' is
| equal to the 64-bit value formed by concatenating `b0' and `b1'.  Otherwise,
| returns 0.
*----------------------------------------------------------------------------*/

INLINE flag eq64( bits32 a0, bits32 a1, bits32 b0, bits32 b1 )
{

    return ( a0 == b0 ) && ( a1 == b1 );

}

/*----------------------------------------------------------------------------
| Returns 1 if the 64-bit value formed by concatenating `a0' and `a1' is less
| than or equal to the 64-bit value formed by concatenating `b0' and `b1'.
| Otherwise, returns 0.
*----------------------------------------------------------------------------*/

INLINE flag le64( bits32 a0, bits32 a1, bits32 b0, bits32 b1 )
{

    return ( a0 < b0 ) || ( ( a0 == b0 ) && ( a1 <= b1 ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the 64-bit value formed by concatenating `a0' and `a1' is less
| than the 64-bit value formed by concatenating `b0' and `b1'.  Otherwise,
| returns 0.
*----------------------------------------------------------------------------*/

INLINE flag lt64( bits32 a0, bits32 a1, bits32 b0, bits32 b1 )
{

    return ( a0 < b0 ) || ( ( a0 == b0 ) && ( a1 < b1 ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the 64-bit value formed by concatenating `a0' and `a1' is not
| equal to the 64-bit value formed by concatenating `b0' and `b1'.  Otherwise,
| returns 0.
*----------------------------------------------------------------------------*/

INLINE flag ne64( bits32 a0, bits32 a1, bits32 b0, bits32 b1 )
{

    return ( a0 != b0 ) || ( a1 != b1 );

}



/*----------------------------------------------------------------------------
| Functions and definitions to determine:  (1) whether tininess for underflow
| is detected before or after rounding by default, (2) what (if anything)
| happens when exceptions are raised, (3) how signaling NaNs are distinguished
| from quiet NaNs, (4) the default generated quiet NaNs, and (4) how NaNs
| are propagated from function inputs to output.  These details are target-
| specific.
*----------------------------------------------------------------------------*/
/* #include "386-Win32-GCC/softfloat-specialize" */

/*============================================================================

This C source fragment is part of the Berkeley SoftFloat IEEE Floating-Point
Arithmetic Package, Release 2c, by John R. Hauser.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TOLERATE ALL LOSSES, COSTS, OR OTHER
PROBLEMS THEY INCUR DUE TO THE SOFTWARE WITHOUT RECOMPENSE FROM JOHN HAUSER OR
THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE, AND WHO FURTHERMORE EFFECTIVELY
INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE
(possibly via similar legal notice) AGAINST ALL LOSSES, COSTS, OR OTHER
PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE, OR
INCURRED BY ANYONE DUE TO A DERIVATIVE WORK THEY CREATE USING ANY PART OF THE
SOFTWARE.

Derivative works require also that (1) the source code for the derivative work
includes prominent notice that the work is derivative, and (2) the source code
includes prominent notice of these three paragraphs for those parts of this
code that are retained.

=============================================================================*/

/*----------------------------------------------------------------------------
| Underflow tininess-detection mode, statically initialized to default value.
| (The declaration in `softfloat.h' must match the `int8' type here.)
*----------------------------------------------------------------------------*/
int8 float_detect_tininess = float_tininess_after_rounding;

/*----------------------------------------------------------------------------
| Raises the exceptions specified by `flags'.  Floating-point traps can be
| defined here if desired.  It is currently not possible for such a trap
| to substitute a result value.  If traps are not implemented, this routine
| should be simply `float_exception_flags |= flags;'.
*----------------------------------------------------------------------------*/

void float_raise( int8 flags )
{

    float_exception_flags |= flags;

}

/*----------------------------------------------------------------------------
| Internal canonical NaN format.
*----------------------------------------------------------------------------*/
typedef struct {
    flag sign;
    bits32 high, low;
} commonNaNT;

/*----------------------------------------------------------------------------
| The pattern for a default generated single-precision NaN.
*----------------------------------------------------------------------------*/
enum {
    float32_default_nan = 0xFFC00000
};

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a NaN;
| otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float32_is_nan( float32 a )
{

    return ( 0xFF000000 < (bits32) ( a<<1 ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a signaling
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float32_is_signaling_nan( float32 a )
{

    return ( ( ( a>>22 ) & 0x1FF ) == 0x1FE ) && ( a & 0x003FFFFF );

}

/*----------------------------------------------------------------------------
| The pattern for a default generated double-precision NaN.  The `high' and
| `low' values hold the most- and least-significant bits, respectively.
*----------------------------------------------------------------------------*/
enum {
    float64_default_nan_high = 0xFFF80000,
    float64_default_nan_low  = 0x00000000
};

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is a NaN;
| otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float64_is_nan( float64 a )
{

    return
           ( 0xFFE00000 <= (bits32) ( a.high<<1 ) )
        && ( a.low || ( a.high & 0x000FFFFF ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is a signaling
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float64_is_signaling_nan( float64 a )
{

    return
           ( ( ( a.high>>19 ) & 0xFFF ) == 0xFFE )
        && ( a.low || ( a.high & 0x0007FFFF ) );

}



/*----------------------------------------------------------------------------
| Takes two double-precision floating-point values `a' and `b', one of which
| is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/

static float64 propagateFloat64NaN( float64 a, float64 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = float64_is_nan( a );
    aIsSignalingNaN = float64_is_signaling_nan( a );
    bIsNaN = float64_is_nan( b );
    bIsSignalingNaN = float64_is_signaling_nan( b );
    a.high |= 0x00080000;
    b.high |= 0x00080000;
    if ( aIsSignalingNaN | bIsSignalingNaN ) float_raise( float_flag_invalid );
    if ( aIsSignalingNaN ) {
        if ( bIsSignalingNaN ) goto returnLargerSignificand;
        return bIsNaN ? b : a;
    }
    else if ( aIsNaN ) {
        if ( bIsSignalingNaN | ! bIsNaN ) return a;
 returnLargerSignificand:
        if ( lt64( a.high<<1, a.low, b.high<<1, b.low ) ) return b;
        if ( lt64( b.high<<1, b.low, a.high<<1, a.low ) ) return a;
        return ( a.high < b.high ) ? a : b;
    }
    else {
        return b;
    }

}





/*----------------------------------------------------------------------------
| Returns the least-significant 32 fraction bits of the double-precision
| floating-point value `a'.
*----------------------------------------------------------------------------*/

INLINE bits32 extractFloat64Frac1( float64 a )
{

    return a.low;

}

/*----------------------------------------------------------------------------
| Returns the most-significant 20 fraction bits of the double-precision
| floating-point value `a'.
*----------------------------------------------------------------------------*/

INLINE bits32 extractFloat64Frac0( float64 a )
{

    return a.high & 0x000FFFFF;

}

/*----------------------------------------------------------------------------
| Returns the exponent bits of the double-precision floating-point value `a'.
*----------------------------------------------------------------------------*/

INLINE int16 extractFloat64Exp( float64 a )
{

    return ( a.high>>20 ) & 0x7FF;

}

/*----------------------------------------------------------------------------
| Returns the sign bit of the double-precision floating-point value `a'.
*----------------------------------------------------------------------------*/

INLINE flag extractFloat64Sign( float64 a )
{

    return a.high>>31;

}

/*----------------------------------------------------------------------------
| Normalizes the subnormal double-precision floating-point value represented
| by the denormalized significand formed by the concatenation of `aSig0' and
| `aSig1'.  The normalized exponent is stored at the location pointed to by
| `zExpPtr'.  The most significant 21 bits of the normalized significand are
| stored at the location pointed to by `zSig0Ptr', and the least significant
| 32 bits of the normalized significand are stored at the location pointed to
| by `zSig1Ptr'.
*----------------------------------------------------------------------------*/

static void
 normalizeFloat64Subnormal(
     bits32 aSig0,
     bits32 aSig1,
     int16 *zExpPtr,
     bits32 *zSig0Ptr,
     bits32 *zSig1Ptr
 )
{
    int8 shiftCount;

    if ( aSig0 == 0 ) {
        shiftCount = countLeadingZeros32( aSig1 ) - 11;
        if ( shiftCount < 0 ) {
            *zSig0Ptr = aSig1>>( - shiftCount );
            *zSig1Ptr = aSig1<<( shiftCount & 31 );
        }
        else {
            *zSig0Ptr = aSig1<<shiftCount;
            *zSig1Ptr = 0;
        }
        *zExpPtr = - shiftCount - 31;
    }
    else {
        shiftCount = countLeadingZeros32( aSig0 ) - 11;
        shortShift64Left( aSig0, aSig1, shiftCount, zSig0Ptr, zSig1Ptr );
        *zExpPtr = 1 - shiftCount;
    }

}

/*----------------------------------------------------------------------------
| Packs the sign `zSign', the exponent `zExp', and the significand formed by
| the concatenation of `zSig0' and `zSig1' into a double-precision floating-
| point value, returning the result.  After being shifted into the proper
| positions, the three fields `zSign', `zExp', and `zSig0' are simply added
| together to form the most significant 32 bits of the result.  This means
| that any integer portion of `zSig0' will be added into the exponent.  Since
| a properly normalized significand will have an integer portion equal to 1,
| the `zExp' input should be 1 less than the desired result exponent whenever
| `zSig0' and `zSig1' concatenated form a complete, normalized significand.
*----------------------------------------------------------------------------*/

INLINE float64
 packFloat64( flag zSign, int16 zExp, bits32 zSig0, bits32 zSig1 )
{
    float64 z;

    z.low = zSig1;
    z.high = ( ( (bits32) zSign )<<31 ) + ( ( (bits32) zExp )<<20 ) + zSig0;
    return z;

}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and extended significand formed by the concatenation of `zSig0', `zSig1',
| and `zSig2', and returns the proper double-precision floating-point value
| corresponding to the abstract input.  Ordinarily, the abstract value is
| simply rounded and packed into the double-precision format, with the inexact
| exception raised if the abstract input cannot be represented exactly.
| However, if the abstract value is too large, the overflow and inexact
| exceptions are raised and an infinity or maximal finite value is returned.
| If the abstract value is too small, the input value is rounded to a
| subnormal number, and the underflow and inexact exceptions are raised if the
| abstract input cannot be represented exactly as a subnormal double-precision
| floating-point number.
|     The input significand must be normalized or smaller.  If the input
| significand is not normalized, `zExp' must be 0; in that case, the result
| returned is a subnormal number, and it must not require rounding.  In the
| usual case that the input significand is normalized, `zExp' must be 1 less
| than the "true" floating-point exponent.  The handling of underflow and
| overflow follows the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float64
 roundAndPackFloat64(
     flag zSign, int16 zExp, bits32 zSig0, bits32 zSig1, bits32 zSig2 )
{
    int8 roundingMode;
    flag roundNearestEven, increment, isTiny;

    roundingMode = float_rounding_mode;
    roundNearestEven = ( roundingMode == float_round_nearest_even );
    increment = ( (sbits32) zSig2 < 0 );
    if ( ! roundNearestEven ) {
        if ( roundingMode == float_round_to_zero ) {
            increment = 0;
        }
        else {
            if ( zSign ) {
                increment = ( roundingMode == float_round_down ) && zSig2;
            }
            else {
                increment = ( roundingMode == float_round_up ) && zSig2;
            }
        }
    }
    if ( 0x7FD <= (bits16) zExp ) {
        if (    ( 0x7FD < zExp )
             || (    ( zExp == 0x7FD )
                  && eq64( 0x001FFFFF, 0xFFFFFFFF, zSig0, zSig1 )
                  && increment
                )
           ) {
            float_raise( float_flag_overflow | float_flag_inexact );
            if (    ( roundingMode == float_round_to_zero )
                 || ( zSign && ( roundingMode == float_round_up ) )
                 || ( ! zSign && ( roundingMode == float_round_down ) )
               ) {
                return packFloat64( zSign, 0x7FE, 0x000FFFFF, 0xFFFFFFFF );
            }
            return packFloat64( zSign, 0x7FF, 0, 0 );
        }
        if ( zExp < 0 ) {
            isTiny =
                   ( float_detect_tininess == float_tininess_before_rounding )
                || ( zExp < -1 )
                || ! increment
                || lt64( zSig0, zSig1, 0x001FFFFF, 0xFFFFFFFF );
            shift64ExtraRightJamming(
                zSig0, zSig1, zSig2, - zExp, &zSig0, &zSig1, &zSig2 );
            zExp = 0;
            if ( isTiny && zSig2 ) float_raise( float_flag_underflow );
            if ( roundNearestEven ) {
                increment = ( (sbits32) zSig2 < 0 );
            }
            else {
                if ( zSign ) {
                    increment = ( roundingMode == float_round_down ) && zSig2;
                }
                else {
                    increment = ( roundingMode == float_round_up ) && zSig2;
                }
            }
        }
    }
    if ( zSig2 ) float_exception_flags |= float_flag_inexact;
    if ( increment ) {
        add64( zSig0, zSig1, 0, 1, &zSig0, &zSig1 );
        zSig1 &= ~ ( ( zSig2 + zSig2 == 0 ) & roundNearestEven );
    }
    else {
        if ( ( zSig0 | zSig1 ) == 0 ) zExp = 0;
    }
    return packFloat64( zSign, zExp, zSig0, zSig1 );

}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand formed by the concatenation of `zSig0' and `zSig1', and
| returns the proper double-precision floating-point value corresponding
| to the abstract input.  This routine is just like `roundAndPackFloat64'
| except that the input significand has fewer bits and does not have to be
| normalized.  In all cases, `zExp' must be 1 less than the "true" floating-
| point exponent.
*----------------------------------------------------------------------------*/

static float64
 normalizeRoundAndPackFloat64(
     flag zSign, int16 zExp, bits32 zSig0, bits32 zSig1 )
{
    int8 shiftCount;
    bits32 zSig2;

    if ( zSig0 == 0 ) {
        zSig0 = zSig1;
        zSig1 = 0;
        zExp -= 32;
    }
    shiftCount = countLeadingZeros32( zSig0 ) - 11;
    if ( 0 <= shiftCount ) {
        zSig2 = 0;
        shortShift64Left( zSig0, zSig1, shiftCount, &zSig0, &zSig1 );
    }
    else {
        shift64ExtraRightJamming(
            zSig0, zSig1, 0, - shiftCount, &zSig0, &zSig1, &zSig2 );
    }
    zExp -= shiftCount;
    return roundAndPackFloat64( zSign, zExp, zSig0, zSig1, zSig2 );

}



/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the double-precision floating-point format.  The conversion is performed
| according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 int32_to_float64( int32 a )
{
    flag zSign;
    bits32 absA;
    int8 shiftCount;
    bits32 zSig0, zSig1;

    if ( a == 0 ) return packFloat64( 0, 0, 0, 0 );
    zSign = ( a < 0 );
    absA = zSign ? - a : a;
    shiftCount = countLeadingZeros32( absA ) - 11;
    if ( 0 <= shiftCount ) {
        zSig0 = absA<<shiftCount;
        zSig1 = 0;
    }
    else {
        shift64Right( absA, 0, - shiftCount, &zSig0, &zSig1 );
    }
    return packFloat64( zSign, 0x412 - shiftCount, zSig0, zSig1 );

}



/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEEE Standard for Floating-Point Arithmetic---
| which means in particular that the conversion is rounded according to the
| current rounding mode.  If `a' is a NaN, the largest positive integer is
| returned.  Otherwise, if the conversion overflows, the largest integer with
| the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

int32 float64_to_int32( float64 a )
{
    flag aSign;
    int16 aExp, shiftCount;
    bits32 aSig0, aSig1, absZ, aSigExtra;
    int32 z;
    int8 roundingMode;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    shiftCount = aExp - 0x413;
    if ( 0 <= shiftCount ) {
        if ( 0x41E < aExp ) {
            if ( ( aExp == 0x7FF ) && ( aSig0 | aSig1 ) ) aSign = 0;
            goto invalid;
        }
        shortShift64Left(
            aSig0 | 0x00100000, aSig1, shiftCount, &absZ, &aSigExtra );
        if ( 0x80000000 < absZ ) goto invalid;
    }
    else {
        aSig1 = ( aSig1 != 0 );
        if ( aExp < 0x3FE ) {
            aSigExtra = aExp | aSig0 | aSig1;
            absZ = 0;
        }
        else {
            aSig0 |= 0x00100000;
            aSigExtra = ( aSig0<<( shiftCount & 31 ) ) | aSig1;
            absZ = aSig0>>( - shiftCount );
        }
    }
    roundingMode = float_rounding_mode;
    if ( roundingMode == float_round_nearest_even ) {
        if ( (sbits32) aSigExtra < 0 ) {
            ++absZ;
            if ( (bits32) ( aSigExtra<<1 ) == 0 ) absZ &= ~1;
        }
        z = aSign ? - absZ : absZ;
    }
    else {
        aSigExtra = ( aSigExtra != 0 );
        if ( aSign ) {
            z = - (   absZ
                    + ( ( roundingMode == float_round_down ) & aSigExtra ) );
        }
        else {
            z = absZ + ( ( roundingMode == float_round_up ) & aSigExtra );
        }
    }
    z = (sbits32) z;
    if ( ( aSign ^ ( z < 0 ) ) && z ) {
 invalid:
        float_raise( float_flag_invalid );
        return aSign ? (sbits32) 0x80000000 : 0x7FFFFFFF;
    }
    if ( aSigExtra ) float_exception_flags |= float_flag_inexact;
    return z;

}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEEE Standard for Floating-Point Arithmetic,
| except that the conversion is always rounded toward zero.  If `a' is a NaN,
| the largest positive integer is returned.  Otherwise, if the conversion
| overflows, the largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

int32 float64_to_int32_round_to_zero( float64 a )
{
    flag aSign;
    int16 aExp, shiftCount;
    bits32 aSig0, aSig1, absZ, aSigExtra;
    int32 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    shiftCount = aExp - 0x413;
    if ( 0 <= shiftCount ) {
        if ( 0x41E < aExp ) {
            if ( ( aExp == 0x7FF ) && ( aSig0 | aSig1 ) ) aSign = 0;
            goto invalid;
        }
        shortShift64Left(
            aSig0 | 0x00100000, aSig1, shiftCount, &absZ, &aSigExtra );
    }
    else {
        if ( aExp < 0x3FF ) {
            if ( aExp | aSig0 | aSig1 ) {
                float_exception_flags |= float_flag_inexact;
            }
            return 0;
        }
        aSig0 |= 0x00100000;
        aSigExtra = ( aSig0<<( shiftCount & 31 ) ) | aSig1;
        absZ = aSig0>>( - shiftCount );
    }
    z = aSign ? - absZ : absZ;
    z = (sbits32) z;
    if ( ( aSign ^ ( z < 0 ) ) && z ) {
 invalid:
        float_raise( float_flag_invalid );
        return aSign ? (sbits32) 0x80000000 : 0x7FFFFFFF;
    }
    if ( aSigExtra ) float_exception_flags |= float_flag_inexact;
    return z;

}

/*----------------------------------------------------------------------------
| Rounds the double-precision floating-point value `a' to an integer,
| and returns the result as a double-precision floating-point value.  The
| operation is performed according to the IEEE Standard for Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_round_to_int( float64 a )
{
    flag aSign;
    int16 aExp;
    bits32 lastBitMask, roundBitsMask;
    int8 roundingMode;
    float64 z;

    aExp = extractFloat64Exp( a );
    if ( 0x413 <= aExp ) {
        if ( 0x433 <= aExp ) {
            if (    ( aExp == 0x7FF )
                 && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) ) {
                return propagateFloat64NaN( a, a );
            }
            return a;
        }
        lastBitMask = 1;
        lastBitMask = ( lastBitMask<<( 0x432 - aExp ) )<<1;
        roundBitsMask = lastBitMask - 1;
        z = a;
        roundingMode = float_rounding_mode;
        if ( roundingMode == float_round_nearest_even ) {
            if ( lastBitMask ) {
                add64( z.high, z.low, 0, lastBitMask>>1, &z.high, &z.low );
                if ( ( z.low & roundBitsMask ) == 0 ) z.low &= ~ lastBitMask;
            }
            else {
                if ( (sbits32) z.low < 0 ) {
                    ++z.high;
                    if ( (bits32) ( z.low<<1 ) == 0 ) z.high &= ~1;
                }
            }
        }
        else if ( roundingMode != float_round_to_zero ) {
            if (   extractFloat64Sign( z )
                 ^ ( roundingMode == float_round_up ) ) {
                add64( z.high, z.low, 0, roundBitsMask, &z.high, &z.low );
            }
        }
        z.low &= ~ roundBitsMask;
    }
    else {
        if ( aExp <= 0x3FE ) {
            if ( ( ( (bits32) ( a.high<<1 ) ) | a.low ) == 0 ) return a;
            float_exception_flags |= float_flag_inexact;
            aSign = extractFloat64Sign( a );
            switch ( float_rounding_mode ) {
             case float_round_nearest_even:
                if (    ( aExp == 0x3FE )
                     && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) )
                   ) {
                    return packFloat64( aSign, 0x3FF, 0, 0 );
                }
                break;
             case float_round_down:
                return
                      aSign ? packFloat64( 1, 0x3FF, 0, 0 )
                    : packFloat64( 0, 0, 0, 0 );
             case float_round_up:
                return
                      aSign ? packFloat64( 1, 0, 0, 0 )
                    : packFloat64( 0, 0x3FF, 0, 0 );
            }
            return packFloat64( aSign, 0, 0, 0 );
        }
        lastBitMask = 1;
        lastBitMask <<= 0x413 - aExp;
        roundBitsMask = lastBitMask - 1;
        z.low = 0;
        z.high = a.high;
        roundingMode = float_rounding_mode;
        if ( roundingMode == float_round_nearest_even ) {
            z.high += lastBitMask>>1;
            if ( ( ( z.high & roundBitsMask ) | a.low ) == 0 ) {
                z.high &= ~ lastBitMask;
            }
        }
        else if ( roundingMode != float_round_to_zero ) {
            if (   extractFloat64Sign( z )
                 ^ ( roundingMode == float_round_up ) ) {
                z.high |= ( a.low != 0 );
                z.high += roundBitsMask;
            }
        }
        z.high &= ~ roundBitsMask;
    }
    if ( ( z.low != a.low ) || ( z.high != a.high ) ) {
        float_exception_flags |= float_flag_inexact;
    }
    return z;

}

/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the double-precision
| floating-point values `a' and `b'.  If `zSign' is 1, the sum is negated
| before being returned.  `zSign' is ignored if the result is a NaN.  The
| addition is performed according to the IEEE Standard for Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

static float64 addFloat64Sigs( float64 a, float64 b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2;
    int16 expDiff;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    bSig1 = extractFloat64Frac1( b );
    bSig0 = extractFloat64Frac0( b );
    bExp = extractFloat64Exp( b );
    expDiff = aExp - bExp;
    if ( 0 < expDiff ) {
        if ( aExp == 0x7FF ) {
            if ( aSig0 | aSig1 ) return propagateFloat64NaN( a, b );
            return a;
        }
        if ( bExp == 0 ) {
            --expDiff;
        }
        else {
            bSig0 |= 0x00100000;
        }
        shift64ExtraRightJamming(
            bSig0, bSig1, 0, expDiff, &bSig0, &bSig1, &zSig2 );
        zExp = aExp;
    }
    else if ( expDiff < 0 ) {
        if ( bExp == 0x7FF ) {
            if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
            return packFloat64( zSign, 0x7FF, 0, 0 );
        }
        if ( aExp == 0 ) {
            ++expDiff;
        }
        else {
            aSig0 |= 0x00100000;
        }
        shift64ExtraRightJamming(
            aSig0, aSig1, 0, - expDiff, &aSig0, &aSig1, &zSig2 );
        zExp = bExp;
    }
    else {
        if ( aExp == 0x7FF ) {
            if ( aSig0 | aSig1 | bSig0 | bSig1 ) {
                return propagateFloat64NaN( a, b );
            }
            return a;
        }
        add64( aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1 );
        if ( aExp == 0 ) return packFloat64( zSign, 0, zSig0, zSig1 );
        zSig2 = 0;
        zSig0 |= 0x00200000;
        zExp = aExp;
        goto shiftRight1;
    }
    aSig0 |= 0x00100000;
    add64( aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1 );
    --zExp;
    if ( zSig0 < 0x00200000 ) goto roundAndPack;
    ++zExp;
 shiftRight1:
    shift64ExtraRightJamming( zSig0, zSig1, zSig2, 1, &zSig0, &zSig1, &zSig2 );
 roundAndPack:
    return roundAndPackFloat64( zSign, zExp, zSig0, zSig1, zSig2 );

}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the double-
| precision floating-point values `a' and `b'.  If `zSign' is 1, the
| difference is negated before being returned.  `zSign' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEEE
| Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float64 subFloat64Sigs( float64 a, float64 b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig0, aSig1, bSig0, bSig1, zSig0, zSig1;
    int16 expDiff;
    float64 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    bSig1 = extractFloat64Frac1( b );
    bSig0 = extractFloat64Frac0( b );
    bExp = extractFloat64Exp( b );
    expDiff = aExp - bExp;
    shortShift64Left( aSig0, aSig1, 10, &aSig0, &aSig1 );
    shortShift64Left( bSig0, bSig1, 10, &bSig0, &bSig1 );
    if ( 0 < expDiff ) goto aExpBigger;
    if ( expDiff < 0 ) goto bExpBigger;
    if ( aExp == 0x7FF ) {
        if ( aSig0 | aSig1 | bSig0 | bSig1 ) {
            return propagateFloat64NaN( a, b );
        }
        float_raise( float_flag_invalid );
        z.low = float64_default_nan_low;
        z.high = float64_default_nan_high;
        return z;
    }
    if ( aExp == 0 ) {
        aExp = 1;
        bExp = 1;
    }
    if ( bSig0 < aSig0 ) goto aBigger;
    if ( aSig0 < bSig0 ) goto bBigger;
    if ( bSig1 < aSig1 ) goto aBigger;
    if ( aSig1 < bSig1 ) goto bBigger;
    return packFloat64( float_rounding_mode == float_round_down, 0, 0, 0 );
 bExpBigger:
    if ( bExp == 0x7FF ) {
        if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
        return packFloat64( zSign ^ 1, 0x7FF, 0, 0 );
    }
    if ( aExp == 0 ) {
        ++expDiff;
    }
    else {
        aSig0 |= 0x40000000;
    }
    shift64RightJamming( aSig0, aSig1, - expDiff, &aSig0, &aSig1 );
    bSig0 |= 0x40000000;
 bBigger:
    sub64( bSig0, bSig1, aSig0, aSig1, &zSig0, &zSig1 );
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aExpBigger:
    if ( aExp == 0x7FF ) {
        if ( aSig0 | aSig1 ) return propagateFloat64NaN( a, b );
        return a;
    }
    if ( bExp == 0 ) {
        --expDiff;
    }
    else {
        bSig0 |= 0x40000000;
    }
    shift64RightJamming( bSig0, bSig1, expDiff, &bSig0, &bSig1 );
    aSig0 |= 0x40000000;
 aBigger:
    sub64( aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1 );
    zExp = aExp;
 normalizeRoundAndPack:
    --zExp;
    return normalizeRoundAndPackFloat64( zSign, zExp - 10, zSig0, zSig1 );

}

/*----------------------------------------------------------------------------
| Returns the result of adding the double-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEEE Standard for
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_add( float64 a, float64 b )
{
    flag aSign, bSign;

    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign == bSign ) {
        return addFloat64Sigs( a, b, aSign );
    }
    else {
        return subFloat64Sigs( a, b, aSign );
    }

}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the double-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEEE Standard for
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_sub( float64 a, float64 b )
{
    flag aSign, bSign;

    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign == bSign ) {
        return subFloat64Sigs( a, b, aSign );
    }
    else {
        return addFloat64Sigs( a, b, aSign );
    }

}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the double-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEEE Standard for
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_mul( float64 a, float64 b )
{
    flag aSign, bSign, zSign;
    int16 aExp, bExp, zExp;
    bits32 aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2, zSig3;
    float64 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    bSig1 = extractFloat64Frac1( b );
    bSig0 = extractFloat64Frac0( b );
    bExp = extractFloat64Exp( b );
    bSign = extractFloat64Sign( b );
    zSign = aSign ^ bSign;
    if ( aExp == 0x7FF ) {
        if (    ( aSig0 | aSig1 )
             || ( ( bExp == 0x7FF ) && ( bSig0 | bSig1 ) ) ) {
            return propagateFloat64NaN( a, b );
        }
        if ( ( bExp | bSig0 | bSig1 ) == 0 ) goto invalid;
        return packFloat64( zSign, 0x7FF, 0, 0 );
    }
    if ( bExp == 0x7FF ) {
        if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
        if ( ( aExp | aSig0 | aSig1 ) == 0 ) {
 invalid:
            float_raise( float_flag_invalid );
            z.low = float64_default_nan_low;
            z.high = float64_default_nan_high;
            return z;
        }
        return packFloat64( zSign, 0x7FF, 0, 0 );
    }
    if ( aExp == 0 ) {
        if ( ( aSig0 | aSig1 ) == 0 ) return packFloat64( zSign, 0, 0, 0 );
        normalizeFloat64Subnormal( aSig0, aSig1, &aExp, &aSig0, &aSig1 );
    }
    if ( bExp == 0 ) {
        if ( ( bSig0 | bSig1 ) == 0 ) return packFloat64( zSign, 0, 0, 0 );
        normalizeFloat64Subnormal( bSig0, bSig1, &bExp, &bSig0, &bSig1 );
    }
    zExp = aExp + bExp - 0x400;
    aSig0 |= 0x00100000;
    shortShift64Left( bSig0, bSig1, 12, &bSig0, &bSig1 );
    mul64To128( aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1, &zSig2, &zSig3 );
    add64( zSig0, zSig1, aSig0, aSig1, &zSig0, &zSig1 );
    zSig2 |= ( zSig3 != 0 );
    if ( 0x00200000 <= zSig0 ) {
        shift64ExtraRightJamming(
            zSig0, zSig1, zSig2, 1, &zSig0, &zSig1, &zSig2 );
        ++zExp;
    }
    return roundAndPackFloat64( zSign, zExp, zSig0, zSig1, zSig2 );

}

/*----------------------------------------------------------------------------
| Returns the result of dividing the double-precision floating-point value `a'
| by the corresponding value `b'.  The operation is performed according to the
| IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_div( float64 a, float64 b )
{
    flag aSign, bSign, zSign;
    int16 aExp, bExp, zExp;
    bits32 aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2;
    bits32 rem0, rem1, rem2, rem3, term0, term1, term2, term3;
    float64 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    bSig1 = extractFloat64Frac1( b );
    bSig0 = extractFloat64Frac0( b );
    bExp = extractFloat64Exp( b );
    bSign = extractFloat64Sign( b );
    zSign = aSign ^ bSign;
    if ( aExp == 0x7FF ) {
        if ( aSig0 | aSig1 ) return propagateFloat64NaN( a, b );
        if ( bExp == 0x7FF ) {
            if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
            goto invalid;
        }
        return packFloat64( zSign, 0x7FF, 0, 0 );
    }
    if ( bExp == 0x7FF ) {
        if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
        return packFloat64( zSign, 0, 0, 0 );
    }
    if ( bExp == 0 ) {
        if ( ( bSig0 | bSig1 ) == 0 ) {
            if ( ( aExp | aSig0 | aSig1 ) == 0 ) {
 invalid:
                float_raise( float_flag_invalid );
                z.low = float64_default_nan_low;
                z.high = float64_default_nan_high;
                return z;
            }
            float_raise( float_flag_divbyzero );
            return packFloat64( zSign, 0x7FF, 0, 0 );
        }
        normalizeFloat64Subnormal( bSig0, bSig1, &bExp, &bSig0, &bSig1 );
    }
    if ( aExp == 0 ) {
        if ( ( aSig0 | aSig1 ) == 0 ) return packFloat64( zSign, 0, 0, 0 );
        normalizeFloat64Subnormal( aSig0, aSig1, &aExp, &aSig0, &aSig1 );
    }
    zExp = aExp - bExp + 0x3FD;
    shortShift64Left( aSig0 | 0x00100000, aSig1, 11, &aSig0, &aSig1 );
    shortShift64Left( bSig0 | 0x00100000, bSig1, 11, &bSig0, &bSig1 );
    if ( le64( bSig0, bSig1, aSig0, aSig1 ) ) {
        shift64Right( aSig0, aSig1, 1, &aSig0, &aSig1 );
        ++zExp;
    }
    zSig0 = estimateDiv64To32( aSig0, aSig1, bSig0 );
    mul64By32To96( bSig0, bSig1, zSig0, &term0, &term1, &term2 );
    sub96( aSig0, aSig1, 0, term0, term1, term2, &rem0, &rem1, &rem2 );
    while ( (sbits32) rem0 < 0 ) {
        --zSig0;
        add96( rem0, rem1, rem2, 0, bSig0, bSig1, &rem0, &rem1, &rem2 );
    }
    zSig1 = estimateDiv64To32( rem1, rem2, bSig0 );
    if ( ( zSig1 & 0x3FF ) <= 4 ) {
        mul64By32To96( bSig0, bSig1, zSig1, &term1, &term2, &term3 );
        sub96( rem1, rem2, 0, term1, term2, term3, &rem1, &rem2, &rem3 );
        while ( (sbits32) rem1 < 0 ) {
            --zSig1;
            add96( rem1, rem2, rem3, 0, bSig0, bSig1, &rem1, &rem2, &rem3 );
        }
        zSig1 |= ( ( rem1 | rem2 | rem3 ) != 0 );
    }
    shift64ExtraRightJamming( zSig0, zSig1, 0, 11, &zSig0, &zSig1, &zSig2 );
    return roundAndPackFloat64( zSign, zExp, zSig0, zSig1, zSig2 );

}

/*----------------------------------------------------------------------------
| Returns the remainder of the double-precision floating-point value `a'
| with respect to the corresponding value `b'.  The operation is performed
| according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_rem( float64 a, float64 b )
{
    flag aSign, bSign, zSign;
    int16 aExp, bExp, expDiff;
    bits32 aSig0, aSig1, bSig0, bSig1, q, term0, term1, term2;
    bits32 allZero, alternateASig0, alternateASig1, sigMean1;
    sbits32 sigMean0;
    float64 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    bSig1 = extractFloat64Frac1( b );
    bSig0 = extractFloat64Frac0( b );
    bExp = extractFloat64Exp( b );
    bSign = extractFloat64Sign( b );
    if ( aExp == 0x7FF ) {
        if (    ( aSig0 | aSig1 )
             || ( ( bExp == 0x7FF ) && ( bSig0 | bSig1 ) ) ) {
            return propagateFloat64NaN( a, b );
        }
        goto invalid;
    }
    if ( bExp == 0x7FF ) {
        if ( bSig0 | bSig1 ) return propagateFloat64NaN( a, b );
        return a;
    }
    if ( bExp == 0 ) {
        if ( ( bSig0 | bSig1 ) == 0 ) {
 invalid:
            float_raise( float_flag_invalid );
            z.low = float64_default_nan_low;
            z.high = float64_default_nan_high;
            return z;
        }
        normalizeFloat64Subnormal( bSig0, bSig1, &bExp, &bSig0, &bSig1 );
    }
    if ( aExp == 0 ) {
        if ( ( aSig0 | aSig1 ) == 0 ) return a;
        normalizeFloat64Subnormal( aSig0, aSig1, &aExp, &aSig0, &aSig1 );
    }
    expDiff = aExp - bExp;
    if ( expDiff < -1 ) return a;
    shortShift64Left(
        aSig0 | 0x00100000, aSig1, 11 - ( expDiff < 0 ), &aSig0, &aSig1 );
    shortShift64Left( bSig0 | 0x00100000, bSig1, 11, &bSig0, &bSig1 );
    q = le64( bSig0, bSig1, aSig0, aSig1 );
    if ( q ) sub64( aSig0, aSig1, bSig0, bSig1, &aSig0, &aSig1 );
    expDiff -= 32;
    while ( 0 < expDiff ) {
        q = estimateDiv64To32( aSig0, aSig1, bSig0 );
        q = ( 4 < q ) ? q - 4 : 0;
        mul64By32To96( bSig0, bSig1, q, &term0, &term1, &term2 );
        shortShift96Left( term0, term1, term2, 29, &term1, &term2, &allZero );
        shortShift64Left( aSig0, aSig1, 29, &aSig0, &allZero );
        sub64( aSig0, 0, term1, term2, &aSig0, &aSig1 );
        expDiff -= 29;
    }
    if ( -32 < expDiff ) {
        q = estimateDiv64To32( aSig0, aSig1, bSig0 );
        q = ( 4 < q ) ? q - 4 : 0;
        q >>= - expDiff;
        shift64Right( bSig0, bSig1, 8, &bSig0, &bSig1 );
        expDiff += 24;
        if ( expDiff < 0 ) {
            shift64Right( aSig0, aSig1, - expDiff, &aSig0, &aSig1 );
        }
        else {
            shortShift64Left( aSig0, aSig1, expDiff, &aSig0, &aSig1 );
        }
        mul64By32To96( bSig0, bSig1, q, &term0, &term1, &term2 );
        sub64( aSig0, aSig1, term1, term2, &aSig0, &aSig1 );
    }
    else {
        shift64Right( aSig0, aSig1, 8, &aSig0, &aSig1 );
        shift64Right( bSig0, bSig1, 8, &bSig0, &bSig1 );
    }
    do {
        alternateASig0 = aSig0;
        alternateASig1 = aSig1;
        ++q;
        sub64( aSig0, aSig1, bSig0, bSig1, &aSig0, &aSig1 );
    } while ( 0 <= (sbits32) aSig0 );
    add64(
        aSig0, aSig1, alternateASig0, alternateASig1, &sigMean0, &sigMean1 );
    if (    ( sigMean0 < 0 )
         || ( ( ( sigMean0 | sigMean1 ) == 0 ) && ( q & 1 ) ) ) {
        aSig0 = alternateASig0;
        aSig1 = alternateASig1;
    }
    zSign = ( (sbits32) aSig0 < 0 );
    if ( zSign ) sub64( 0, 0, aSig0, aSig1, &aSig0, &aSig1 );
    return
        normalizeRoundAndPackFloat64( aSign ^ zSign, bExp - 4, aSig0, aSig1 );

}

/*----------------------------------------------------------------------------
| Returns the square root of the double-precision floating-point value `a'.
| The operation is performed according to the IEEE Standard for Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_sqrt( float64 a )
{
    flag aSign;
    int16 aExp, zExp;
    bits32 aSig0, aSig1, zSig0, zSig1, zSig2, doubleZSig0;
    bits32 rem0, rem1, rem2, rem3, term0, term1, term2, term3;
    float64 z;

    aSig1 = extractFloat64Frac1( a );
    aSig0 = extractFloat64Frac0( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    if ( aExp == 0x7FF ) {
        if ( aSig0 | aSig1 ) return propagateFloat64NaN( a, a );
        if ( ! aSign ) return a;
        goto invalid;
    }
    if ( aSign ) {
        if ( ( aExp | aSig0 | aSig1 ) == 0 ) return a;
 invalid:
        float_raise( float_flag_invalid );
        z.low = float64_default_nan_low;
        z.high = float64_default_nan_high;
        return z;
    }
    if ( aExp == 0 ) {
        if ( ( aSig0 | aSig1 ) == 0 ) return packFloat64( 0, 0, 0, 0 );
        normalizeFloat64Subnormal( aSig0, aSig1, &aExp, &aSig0, &aSig1 );
    }
    zExp = ( ( aExp - 0x3FF )>>1 ) + 0x3FE;
    aSig0 |= 0x00100000;
    shortShift64Left( aSig0, aSig1, 11, &term0, &term1 );
    zSig0 = ( estimateSqrt32( aExp, term0 )>>1 ) + 1;
    if ( zSig0 == 0 ) zSig0 = 0x7FFFFFFF;
    doubleZSig0 = zSig0 + zSig0;
    shortShift64Left( aSig0, aSig1, 9 - ( aExp & 1 ), &aSig0, &aSig1 );
    mul32To64( zSig0, zSig0, &term0, &term1 );
    sub64( aSig0, aSig1, term0, term1, &rem0, &rem1 );
    while ( (sbits32) rem0 < 0 ) {
        --zSig0;
        doubleZSig0 -= 2;
        add64( rem0, rem1, 0, doubleZSig0 | 1, &rem0, &rem1 );
    }
    zSig1 = estimateDiv64To32( rem1, 0, doubleZSig0 );
    if ( ( zSig1 & 0x1FF ) <= 5 ) {
        if ( zSig1 == 0 ) zSig1 = 1;
        mul32To64( doubleZSig0, zSig1, &term1, &term2 );
        sub64( rem1, 0, term1, term2, &rem1, &rem2 );
        mul32To64( zSig1, zSig1, &term2, &term3 );
        sub96( rem1, rem2, 0, 0, term2, term3, &rem1, &rem2, &rem3 );
        while ( (sbits32) rem1 < 0 ) {
            --zSig1;
            shortShift64Left( 0, zSig1, 1, &term2, &term3 );
            term3 |= 1;
            term2 |= doubleZSig0;
            add96( rem1, rem2, rem3, 0, term2, term3, &rem1, &rem2, &rem3 );
        }
        zSig1 |= ( ( rem1 | rem2 | rem3 ) != 0 );
    }
    shift64ExtraRightJamming( zSig0, zSig1, 0, 10, &zSig0, &zSig1, &zSig2 );
    return roundAndPackFloat64( 0, zExp, zSig0, zSig1, zSig2 );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is equal to
| the corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_eq( float64 a, float64 b )
{

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        if ( float64_is_signaling_nan( a ) || float64_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    return
           ( a.low == b.low )
        && (    ( a.high == b.high )
             || (    ( a.low == 0 )
                  && ( (bits32) ( ( a.high | b.high )<<1 ) == 0 ) )
           );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than
| or equal to the corresponding value `b', and 0 otherwise.  The comparison is
| performed according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_le( float64 a, float64 b )
{
    flag aSign, bSign;

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign != bSign ) {
        return
               aSign
            || (    ( ( (bits32) ( ( a.high | b.high )<<1 ) ) | a.low | b.low )
                 == 0 );
    }
    return
          aSign ? le64( b.high, b.low, a.high, a.low )
        : le64( a.high, a.low, b.high, b.low );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_lt( float64 a, float64 b )
{
    flag aSign, bSign;

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign != bSign ) {
        return
               aSign
            && (    ( ( (bits32) ( ( a.high | b.high )<<1 ) ) | a.low | b.low )
                 != 0 );
    }
    return
          aSign ? lt64( b.high, b.low, a.high, a.low )
        : lt64( a.high, a.low, b.high, b.low );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is equal to
| the corresponding value `b', and 0 otherwise.  The invalid exception is
| raised if either operand is a NaN.  Otherwise, the comparison is performed
| according to the IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_eq_signaling( float64 a, float64 b )
{

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    return
           ( a.low == b.low )
        && (    ( a.high == b.high )
             || (    ( a.low == 0 )
                  && ( (bits32) ( ( a.high | b.high )<<1 ) == 0 ) )
           );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than or
| equal to the corresponding value `b', and 0 otherwise.  Quiet NaNs do not
| cause an exception.  Otherwise, the comparison is performed according to the
| IEEE Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_le_quiet( float64 a, float64 b )
{
    flag aSign, bSign;

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        if ( float64_is_signaling_nan( a ) || float64_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign != bSign ) {
        return
               aSign
            || (    ( ( (bits32) ( ( a.high | b.high )<<1 ) ) | a.low | b.low )
                 == 0 );
    }
    return
          aSign ? le64( b.high, b.low, a.high, a.low )
        : le64( a.high, a.low, b.high, b.low );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  Quiet NaNs do not cause an
| exception.  Otherwise, the comparison is performed according to the IEEE
| Standard for Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

flag float64_lt_quiet( float64 a, float64 b )
{
    flag aSign, bSign;

    if (    (    ( extractFloat64Exp( a ) == 0x7FF )
              && ( extractFloat64Frac0( a ) | extractFloat64Frac1( a ) ) )
         || (    ( extractFloat64Exp( b ) == 0x7FF )
              && ( extractFloat64Frac0( b ) | extractFloat64Frac1( b ) ) )
       ) {
        if ( float64_is_signaling_nan( a ) || float64_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    aSign = extractFloat64Sign( a );
    bSign = extractFloat64Sign( b );
    if ( aSign != bSign ) {
        return
               aSign
            && (    ( ( (bits32) ( ( a.high | b.high )<<1 ) ) | a.low | b.low )
                 != 0 );
    }
    return
          aSign ? lt64( b.high, b.low, a.high, a.low )
        : lt64( a.high, a.low, b.high, b.low );

}

