/*
 * NAME: longmath.h
 *
 * DESCRIPTION:
 *	Definitions and prototypes for longmath.c.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/12/07: lmEq() and lmRatEq() prototypes added
 */


#define LMINCLUDED

/* radix2Type must be big enough for RADIX^2-1 (RADIX defined in longmath.c) */
typedef unsigned int radix2Type;

/* number of RADIX digits in one lmNumberType */
#define NDIGRDX 15

typedef struct {
  short int neg, len;
  radix2Type r[NDIGRDX];
} lmNumberType;

extern void	lmPrint();
extern int	lmSign();
extern void	lmNegate();
extern double	lmReal();
extern int	lmLt();
extern int	lmRatLt();
extern void	lmCopy();
extern void	lmMult();
extern void	lmAdd();
extern void	lmSub();
extern void	lmStore();
extern int	lmDet31();
extern int	lmDet41();
extern int	lmEq();
extern int	lmRatEq();
extern int      lmPositive();
extern void     lmExchange();

