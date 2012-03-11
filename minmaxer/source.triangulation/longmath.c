/*
 * NAME: longmath.c
 *
 * DESCRIPTION:
 *	Included in this file are routines which support extended 
 *	precision integer math, based upon algorithms from Knuth's Art
 *	of Computer Programming Vol. 2.  Design trade-off's were made
 *	primarily in favor of speed.  The number representation used
 *	by these routines is of fixed size, thus one must know in advance
 *	what the absolute value of the maximum number to manipulate will
 *	be.  The compensating advantage of this limitation is that dynamic
 *	memory handling can be avoided.
 *
 * NUMBER REPRESENTATION:
 *	Numbers are represented with three parts (see lmNumberType in
 *	longmath.h).  Neg is 1 or 0 to represent negative and positive
 *	numbers, respectively.  Len is the number of RADIX sized digits
 *	needed to represent the number, with 0 needing 0 digits by
 *	convention.  The third part of each lmNumberType is the array
 *	r used to store the digits.  r[0] is the least significant digit
 *	(Knuth numbers his digits in the opposite order).  The following
 *	algorithms are set up expecting each element in r to be able
 *	to store RADIX^2-1, rather than just RADIX-1, the minimum.  This
 *	makes the algorithms a little simpler, at the expense of storage
 *	space.
 *
 *	The largest number that can be stored is RADIX^NDIGRDX - 1.
 *	Since the numbers are represented as signed quantities, any
 *	number with an absolute value less than or equal to the
 *	limit above can be stored.
 *
 * HISTORY:
 *	89/09/02: current version
 *	89/09/03: addToStr uses LOGRDX instead of always 5
 *	89/10/04: lmMult fixed, would run 1 word beyond with carry
 *	89/10/09: lmDet31 calls in lmDet41 now with (void) casts
 *	89/10/26: cosmetic changes to make consistant with other files
 *	89/12/06: lmEq added
 *	89/12/07: lmRatEq added
 *
 * BUGS:
 *	lmNumbers cannot be subtracted from themselves. Use lmStore(rp, 0)
 *	to clear a register.
 *	The destination lmNumber for lmMult cannot be the same as either
 *	of the source registers.
 *
 * SEE ALSO:
 *	Knuth, D.E. "The Art of Computer Programming, Vol II:  Seminumerical
 *	Numerical Algorithms."  Addison-Wesley, Reading, Mass., 1973.
 *	Pages 250-255 and 302-309.
 */

#include "longmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*
 * Definitions which are dependent upon the size of RADIX (RADIX must
 * be a power of 2).
 */
#ifdef WORDSIZE16
#define RADIX 0x0100			/* 2^(wordsize/2)		*/
#define RDXLG2 8			/* wordsize/2			*/
#define LOGRDX 3			/* ceiling(log10 RADIX)		*/
#define LOGRDXFMT "%.3d"		/* in addToStr, same as LOGRDX	*/

#else  /* default is word size of 32 bits */
#define RADIX 0x00010000
#define RDXLG2 16
#define LOGRDX 5
#define LOGRDXFMT "%.5d"
#endif

/*
 * Definitions that would not have to be modified if RADIX changed
 */
#define RMASK (RADIX - 1)
#define MODRDX(x) ((x) & RMASK)		/* modula			*/
#define DIVRDX(x) ((x) >> RDXLG2)	/* floor(x / RADIX)		*/
#define NDIG10 (NDIGRDX * LOGRDX + 2)	/* chars to represent lmNumber	*/

#define MAX(x1, x2) ((x1) > (x2) ? (x1) : (x2))


void lmAbort(s)
char *s;
{
  (void) fprintf(stdout, "%s\n", s);
  (void) fprintf(stdout, "longmath bailing out ... sorry\n");
  exit(-1);
}  /* -------------------- lmAbort -------------------- */


void addToStr(str, n)
char *str;
radix2Type n;
{
  static char str1[LOGRDX + 1];
  int i, carry, sum;
  char *s, *s1;
/*  char *sprintf(); */

  /* build up string representation of number to be added */
  (void) sprintf(str1, LOGRDXFMT, n);
  for (i=0; i < LOGRDX; i++) str1[i] &= 15;

  /* do decimal addition */
  carry = 0;
  s = str + NDIG10 - 2;
  s1 = str1 + LOGRDX -1;
  for (i=0; i < LOGRDX; i++) {
    sum = *s1-- + *s;
    if (carry) sum++;
    carry = sum >= 10;
    *s-- = sum % 10;
  }
  
  while (carry) {		/* propagate the carry */
    (*s)++;
    if (carry = (*s >= 10)) *(s--) %= 10;
  }
}  /* -------------------- addToStr -------------------- */


void lmPrint(fp, rp)
FILE         *fp;
lmNumberType *rp;
/*
 *   Generate a string with the decimal representation of the register
 * pointed to and print it.
 */
{
  int i, j, p, carry;
  static char str[NDIG10];
  char *s;

  /* initialize string for decimal representation */
  s = str;
  for (i=0; i < NDIG10; i++) *s++ = 0;

  /* build decimal character representation one RADIX word at a time */
  for (i= rp->len - 1; i > 0; i--) {
    /* add value in reg part i to the decimal string representation */
    addToStr(str, rp->r[i]);
    
    /* multiply str by RADIX */
    carry = 0;
    for (j= NDIG10 - 2; j >=0; j--) {
      p = str[j] * RADIX + carry;
      str[j] = p % 10;
      carry = p / 10;
    }
  }
  addToStr(str, *(rp->r));

  /* convert numbers into character values */
  s = str;
  for (i=0; i < (NDIG10 - 1); i++) *s++ += '0';

  /* advance back over leading zeros */
  s = str;
  while (*s == '0') {
    s++;
    if (! *(s + 1)) break;	/* leave at least one digit to print */
  }

  /* add a negative sign if necessary */
  if (rp->neg) *(--s) = '-';

  (void) fprintf(fp, "%s", s);
}  /* -------------------- lmPrint -------------------- */ 


int lmSign(rp)
lmNumberType *rp;
{
  if (rp->neg)
    return -1;
  else if (rp->len)
    return 1;
  else
    return 0;
}  /* -------------------- lmSign -------------------- */


void lmNegate(rp)
lmNumberType *rp;
{
  lmNumberType lmZero;
  lmStore (&lmZero, 0);
  if (! lmEq (&lmZero, rp)) rp->neg ^= 1;
}  /* -------------------- lmNegate -------------------- */


double lmReal(rp)
lmNumberType *rp;
/*   Return a double type representation of a longmath number */
{
  double x = 0;
  int i;

  for (i= rp->len - 1; i >= 0; i--) {
    x *= RADIX;
    x += rp->r[i];
  }
  if (rp->neg) x = -x;
  return x;
}  /* -------------------- lmReal -------------------- */


int lmLt(r1p, r2p)
lmNumberType *r1p, *r2p;
/* returns true if register pointed to by arg1 is less than that of arg2 */
{
  int i, lt = 0;

  /* if signs different, first arg is less if it is negative */
  if (r1p->neg != r2p->neg) return(r1p->neg);

  if (r1p->len != r2p->len) {		/* if lengths are different */
    lt = r1p->len < r2p->len;		/* then lt = arg1 shorter than arg2 */
    if (r1p->neg) lt = ! lt;		/* flip result if both args neg */
  }

  else {				/* else (lengths equal) */
    if (i = r1p->len) {			/* if (both) lengths != 0 */
      do {				/* find most significant difference */
        i--;
        if (r1p->r[i] != r2p->r[i]) break;
      } while (i);

      lt = r1p->r[i] < r2p->r[i];
      if (r1p->r[i] != r2p->r[i])	/* if the two lmNumbers not equal */
        if (r1p->neg) lt = ! lt;	/*   and both neg, then flip result */
    }
    else				/* else both lengths 0 */
      lt = 0;				/* return (0 < 0) */
  }

  return lt;
}  /* -------------------- lmLt -------------------- */


int lmRatLt(num1p, den1p, num2p, den2p)
lmNumberType *num1p, *den1p, *num2p, *den2p;
/*
 * Returns:  num1/den1 < num2/den2
 *   This routine evaluates the equivalent form: num1*den2 < num2*den1 
 * (It is assumed that the denominators are greater than zero.)
 */
{
  lmNumberType lhs, rhs;

/* (void) printf("lmRatLt:  ");
lmPrint(stdout, num1p); (void) printf("/");
lmPrint(stdout, den1p); (void) printf(" < ");
lmPrint(stdout, num2p); (void) printf("/");
lmPrint(stdout, den2p); (void) printf("\n");
*/

  lmMult(num1p, den2p, &lhs);
  lmMult(den1p, num2p, &rhs);

  return(lmLt(&lhs, &rhs));
}  /* -------------------- lmRatLt -------------------- */

  
void lmCopy(srcp, dstp)
lmNumberType *srcp, *dstp;
/*
 *   Copy contents from src register to dst register, given pointers to each.
 */
{
  int i;
  radix2Type *src, *dst;

  src = srcp->r;
  dst = dstp->r;
  for (i=0; i < NDIGRDX; i++) *dst++ = *src++;

  dstp->neg = srcp->neg;
  dstp->len = srcp->len;
}  /* -------------------- lmCopy -------------------- */


void lmMult(src1p, src2p, dstp)		/* dst <=  src1 * src2  */
lmNumberType *src1p, *src2p, *dstp;
{
  int i, j, k;
  radix2Type carry = 0;
  radix2Type *src1, *src2, *dst;
  static char *multAbortMess = { "Multiply overflow!" };

  /* clear destination register */
  dst = dstp->r;
  for (i=0; i < NDIGRDX; i++) *dst++ = 0;

  /* set new parameters for dst register */
  dstp->neg = src1p->neg ^ src2p->neg;

  /* special case if either operand is 0 */
  if (! src1p->len  || ! src2p->len) {
    dstp->len = 0;
    dstp->neg = 0;
    return;
  }

  if ((dstp->len = src1p->len + src2p->len - 1) > NDIGRDX)
    lmAbort(multAbortMess);

  /* for each digit in src2 */
  src2 = src2p->r;
  for (j=0; j < src2p->len; j++) {
    k = j;
    dst = &dstp->r[j];
    if (! *src2) {		/* skip on zero multipliers */
      src2++; 
      continue;
    } 
    /* for each digit it src1 */
    carry = 0;
    src1 = src1p->r;
    for (i=0; i < src1p->len; i++) {
      *dst += *src1++ * *src2 + carry;
      carry = *dst >> RDXLG2;
      *dst++ &= RMASK;
      k++;
    }
    src2++;
    if (k < NDIGRDX) *dst = carry;
  }

  if (carry) 
    if (++(dstp->len) > NDIGRDX) lmAbort(multAbortMess);
}  /* -------------------- lmMult -------------------- */


void lmAdd(srcp, dstp)			/* dst <=  dst + src */
lmNumberType *srcp, *dstp;
{
  register int i, len;
  radix2Type carry = 0;
  int borrow = 0;
  radix2Type *src, *dst;

  src = srcp->r;
  dst = dstp->r;
  len = MAX(srcp->len, dstp->len);

  /* if signs are different, do subtract, else add */
  if (srcp->neg ^ dstp->neg) {
    for (i=0; i<len; i++) {
      /* order of following 2 statements important, *dst can't go negative */
      *dst += RADIX;
      *dst -= *src++;
      if (borrow) (*dst)--;
      borrow = (*dst < RADIX);
      *dst++ &= RMASK;
    }

    /* if the difference was negative, change sign and complement number */
    if (borrow) {
      borrow = 0;
      dstp->neg ^= 1;
      dst = dstp->r;
      for (i=0; i<len; i++) {
        *dst = RADIX - *dst;
        if (borrow) (*dst)--;
        borrow = *dst < RADIX;
        *dst++ &= RMASK;
      }
    }
    /* Determine the new length of the dst register:
     * if len is 0, then both arguments were 0, and len values don't need
     * to be modified.
     */
    if (len) {
      dst = &(dstp->r[len]);
      while (! *(--dst))
        if (!--len) break;   /* <====== */

      dstp->len = len;

      /* ensure sign of 0 (which has length zero) is not negative */
      if (! dstp->len) dstp->neg = 0;
    }
  }

  /* else signs are the same: add absolute values */
  else {
    for (i=0; i<len; i++) {
      *dst += *src++;
      if (carry) (*dst)++;
      carry = *dst & RADIX;
      *dst++ &= RMASK;
    }
    dstp->len = len;
    if (carry) {
      if (len == NDIGRDX)
        lmAbort("Addition overflow!");
      (*dst) = 1;
      dstp->len++;
    }
  }
}  /* -------------------- lmAdd -------------------- */


void lmSub(srcp, dstp)			/* dst <=  dst - src */
lmNumberType *srcp, *dstp;
{
  if (srcp == dstp)
    lmAbort("lmSub: cannot subtract number from itself");

  srcp->neg ^= 1;			/* flip sign of src */
  lmAdd(srcp, dstp);
  srcp->neg ^= 1;			/* flip sign back */
}  /* -------------------- lmSub -------------------- */


void lmStore(rp, n)
lmNumberType *rp;
int n;
{
  register int i;

  /* clear the register and set neg */
  for (i=0; i < NDIGRDX; i++) rp->r[i] = 0;
  if (n >= 0) {
    rp->neg = 0;
  }
  else {
    n = -n;
    rp->neg = 1;
  }

  /* set digits and length */
  i = 0;
  while (n) {
    if (i == NDIGRDX) lmAbort("lmStore: number too big to store");
    rp->r[i++] = n & RMASK;
    n >>= RDXLG2; 
  }
  rp->len = i;
}  /* -------------------- lmStore -------------------- */


int lmDet31(p0, p1, p2, p3, p4, p5, dstp)
lmNumberType *p0, *p1, *p2, *p3, *p4, *p5, *dstp;
/* 
 * Calculate:
 *   | p0 p1 1 |
 *   | p2 p3 1 | => dst
 *   | p4 p5 1 |
 *
 * dst <= p0(p3-p5) + p2(p5-p1) + p4(p1-p3)
 *
 * Return: sign(dstp)
 */
{
  lmNumberType tmp0, tmp1;

  /*				tmp0	  tmp1		dst	*/
  lmCopy(p3, &tmp0);	/*	p3				*/
  lmSub(p5, &tmp0);	/* 	p3-p5				*/
  lmMult(p0, &tmp0, &tmp1); /*		p0(p3-p5)		*/

  lmCopy(p5, &tmp0);	/*	p5				*/
  lmSub(p1, &tmp0);	/*	p5-p1				*/
  lmMult(p2, &tmp0, dstp); /*			p2(p5-p1)	*/
  lmAdd(&tmp1, dstp);	/*			p0(p3-p5) + p2(p5-p1) */

  lmCopy(p1, &tmp0);	/*	p1				*/
  lmSub(p3, &tmp0);	/*	p1-p3				*/
  lmMult(p4, &tmp0, &tmp1); /*		p4(p1-p3)		*/
  lmAdd(&tmp1, dstp);	/*		    p0(p3-p5)+p2(p5-p1)+p4(p1-p3) */

  return lmSign(dstp);
}  /* -------------------- lmDet31 -------------------- */

int lmDet41(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, dstp)
lmNumberType *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;
lmNumberType *p9, *p10, *p11, *dstp;
/*
 * Calculate:
 *   | p0  p1  p2  1 |
 *   | p3  p4  p5  1 |
 *   | p6  p7  p8  1 |  => dst
 *   | p9  p10 p11 1 |
 *
 *            |p4  p5  1|      |p1  p2  1|      |p1  p2  1|      |p1  p2  1|
 *  dst <= p0*|p7  p8  1| - p3*|p7  p8  1| + p6*|p4  p5  1| - p9*|p4  p5  1|
 *            |p10 p11 1|      |p10 p11 1|      |p10 p11 1|      |p7  p8  1|
 *
 *         p0 *   Da      - p3 *   Db      + p6 *   Dc      - p9 *   Dd
 *
 * Return: sign(dstp)
 */ 
{
  lmNumberType tmp2, tmp3;

  /*					       tmp2	tmp3	 dst	*/
  (void) lmDet31(p4,p5,p7,p8,p10,p11, &tmp2);/*	Da			*/
  lmMult(p0, &tmp2, dstp);		/*			p0*Da	*/
  (void) lmDet31(p1,p2,p7,p8,p10,p11, &tmp2);/*	Db			*/
  lmMult(p3, &tmp2, &tmp3);		/*		p3*Db		*/
  lmSub(&tmp3, dstp);			/*			p0*Da-p3*Db */
  (void) lmDet31(p1,p2,p4,p5,p10,p11, &tmp2);/*	Dc			*/
  lmMult(p6, &tmp2, &tmp3);		/*		p6*Dc		*/
  lmAdd(&tmp3, dstp);			/*		   p0*Da-p3*Db+p6*Dc */
  (void) lmDet31(p1,p2,p4,p5,p7,p8,   &tmp2);/*	Dd			*/
  lmMult(p9, &tmp2, &tmp3);		/*		p9*Dd		*/
  lmSub(&tmp3, dstp);			/* dst = p0*Da-p3*Db+p6*Dc-p9*Dd */

  return lmSign(dstp);
}  /* -------------------- lmDet41 -------------------- */


int lmEq(r1p, r2p)
lmNumberType *r1p, *r2p;
{
  int i;

  if (r1p->len != r2p->len)
    return 0;

  if (r1p->neg != r2p->neg)
    return 0;

  for (i=0; i < r1p->len; i++)
    if (r1p->r[i] != r2p->r[i])
      return 0;

  return 1;
}  /* -------------------- lmEq -------------------- */


int lmRatEq(num1p, den1p, num2p, den2p)
lmNumberType *num1p, *den1p, *num2p, *den2p;
/*
 * Returns:  num1/den1 == num2/den2
 *   This routine evaluates the equivalent form: num1*den2 == num2*den1 
 * (It is assumed that the denominators are greater than zero.)
 */
{
  lmNumberType lhs, rhs;

/* (void) printf("lmRatEq:  ");
lmPrint(stdout, num1p); (void) printf("/");
lmPrint(stdout, den1p); (void) printf(" < ");
lmPrint(stdout, num2p); (void) printf("/");
lmPrint(stdout, den2p); (void) printf("\n");
*/

  lmMult(num1p, den2p, &lhs);
  lmMult(den1p, num2p, &rhs);

  return(lmEq(&lhs, &rhs));
}  /* -------------------- lmRatLt -------------------- */


int 
lmPositive(longMathNumber)
     lmNumberType *longMathNumber;
{
  return lmSign(longMathNumber) != -1;

} /* -------------------- lmPositive -------------------- */

void
lmExchange(num1, num2)
     lmNumberType *num1, *num2;
{
  static lmNumberType temp;

  temp  = *num1;
  *num1 = *num2;
  *num2 = temp;

} /* -------------------- lmExchange -------------------- */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int 
lmDet22 (p0, p1, p2, p3, dstp)

     lmNumberType *p0, *p1, *p2, *p3, *dstp;

/* 
 * Calculate:
 *   | p0 p1 |
 *   | p2 p3 | => dst
 *
 * dst <= p0p3 - p2p1
 *
 * Return: sign(dstp)
 */

{
  lmNumberType tmp0;

  lmMult(p0, p3, dstp);
  lmMult(p2, p1, &tmp0);
  lmSub(&tmp0, dstp);

  return lmSign(dstp);
}

/*---------------------------------------------------------------------------*/

int 
lmDet33 (p0, p1, p2, p3, p4, p5, p6, p7, p8, dstp)

     lmNumberType *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *dstp;

/* 
 * Calculate:
 *   | p0 p1 p6 |
 *   | p2 p3 p7 | => dst
 *   | p4 p5 p8 |
 *
 * dst <= p0(p3p8-p5p7) + p2(p5p6-p1p8) + p4(p1p7-p3p6)
 *
 * Return: sign(dstp)
 */

/* 
 * Calculate:
 *   | p0 p1 p2 |
 *   | p3 p4 p5 | => dst
 *   | p6 p7 p8 |
 *
 * dst <= p0(p4p8-p7p5) + p3(p7p2-p1p8) + p6(p1p5-p4p2)
 *
 * Return: sign(dstp)
 */

{
  lmNumberType tmp0, tmp1;

  /*				tmp0	  tmp1		dstp	*/

  lmMult(p4, p8, &tmp0);/*	p4p8				*/
  lmMult(p7, p5, &tmp1);/*      p4p8      p7p5                  */
  lmSub(&tmp1, &tmp0);	/* 	p4p8-p7p5        		*/
  lmMult(p0, &tmp0, dstp); /*		             p0(p4p8-p7p5)*/
 
  lmMult(p7, p2, &tmp0);/*	p7p2				*/
  lmMult(p1, p8, &tmp1);/*      p7p2      p1p8                  */
  lmSub(&tmp1, &tmp0);	/* 	p7p2-p1p8        		*/
  lmMult(p3, &tmp0, &tmp1); /*		p3(p7p2-p1p8) p0(p4p8-p7p5)*/
  lmAdd(&tmp1, dstp);/*                            p3(p7p2-p1p8)+p0(p4p8-p7p5)*/

  lmMult(p1, p5, &tmp0);/*	p1p5				*/
  lmMult(p4, p2, &tmp1);/*      p1p5      p4p2                  */
  lmSub(&tmp1, &tmp0);	/* 	p1p5-p4p2        		*/
  lmMult(p6, &tmp0, &tmp1); /*		p6(p1p5-p4p2)    ditto  */
  lmAdd(&tmp1, dstp);/*                            p6(p1p5-p4p2)+p3(p7p2-p1p8)+p0(p4p8-p7p5)*/

  return lmSign(dstp);
}

/*---------------------------------------------------------------------------*/

int 
lmDet21 (p0, p1, dstp)

     lmNumberType *p0, *p1, *dstp;

/* 
 * Calculate:
 *   | p0 1 |
 *   | p1 1 | => dst
 *
 * dst <= p0 - p1
 *
 * Return: sign(dstp)
 */
{
  lmNumberType tmp0, tmp1;


  lmCopy(p0, dstp);
  lmSub(p1, dstp);	

  return lmSign(dstp);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
