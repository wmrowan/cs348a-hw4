/*
 * NAME: LongMath.MinMax.c
 *
 * DESCRIPTION:
 *	Contains Longmath functions for the slope of a triangle.
 *
 * HISTORY:
 *	90/02/22: Generate this File.
 */

#include <stdio.h>
#include <math.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "triation.h"

#define MAX(a, b) (a > b ? a : b)
  
static
lmComputeABC (vertex3dType*, vertex3dType*, vertex3dType*, 
              lmNumberType*, lmNumberType*, lmNumberType*, lmNumberType*, lmNumberType*);

static
lmComputeLexicographicalOrderSlope(vertex3dType*, vertex3dType*, vertex3dType*, slopeType*);

/*---------------------------------------------------------------------------*/

void
lmMake3dVertex(vertex, index, xCoord, yCoord, zCoord)

     vertex3dType *vertex;
     indexType index;
     coordType xCoord, yCoord, zCoord;

{
  lmStore(&vertex->x, xCoord);
  lmStore(&vertex->y, yCoord);
  lmStore(&vertex->z, zCoord);
  vertex->a = index;
}

/*---------------------------------------------------------------------------*/

lmComputeSlope (originVertex, fromVertex, toVertex, slope)
     vertex3dType  *originVertex, *fromVertex, *toVertex;
     slopeType *slope;
{
    lmComputeABC (originVertex, fromVertex, toVertex,
		  &slope->A, &slope->B, &slope->C, &slope->C2,
		  &slope->A2plusB2);

  lmComputeLexicographicalOrderSlope (originVertex, fromVertex, 
				       toVertex, slope);
}

/*---------------------------------------------------------------------------*/

static
lmComputeABC (originVertex, fromVertex, toVertex, Ap, Bp, Cp, C2p, A2plusB2p)

     vertex3dType *originVertex, *fromVertex, *toVertex;
     lmNumberType *A2plusB2p, *Ap, *Bp, *Cp, *C2p;

{
  lmNumberType B2;

  (void) lmDet31 (&originVertex->y, &originVertex->z, 
		  &toVertex->y, &toVertex->z,
		  &fromVertex->y, &fromVertex->z,
		  Ap);
  lmMult (Ap, Ap, A2plusB2p);

  (void) lmDet31 (&originVertex->x, &originVertex->z, 
		  &toVertex->x, &toVertex->z,
		  &fromVertex->x, &fromVertex->z,
		  Bp);
  lmNegate (Bp);
  lmMult (Bp, Bp, &B2);
  
  lmAdd (&B2, A2plusB2p);

  (void) lmDet31 (&originVertex->x, &originVertex->y, 
		  &toVertex->x, &toVertex->y,
		  &fromVertex->x, &fromVertex->y,
		  Cp);
  lmMult (Cp, Cp, C2p);
}

/*---------------------------------------------------------------------------*/

static
lmComputeLexicographicalOrderSlope(originVertex, fromVertex, toVertex, slope)
     vertex3dType *originVertex, *fromVertex, *toVertex;
     slopeType  *slope;
{
  slope->lexHigh = originVertex->a;
  slope->lexMed = fromVertex->a;
  slope->lexLow = toVertex->a;

}

/*---------------------------------------------------------------------------*/

static int
lmTestIfAnchor (slope, candidate, v1, v2)

     slopeType *slope;
     vertex3dType *candidate, *v1, *v2;

{
  lmNumberType tmp, tmp2, tmp3, tmp4;
  static lmNumberType lmZero;
  
  lmCopy (&v1->y, &tmp);
  lmSub  (&candidate->y, &tmp);
  lmMult (&slope->A, &tmp, &tmp4);

  lmCopy (&v1->x, &tmp);
  lmSub  (&candidate->x, &tmp);
  lmMult (&slope->B, &tmp, &tmp3);

  lmSub (&tmp3, &tmp4);

  lmCopy (&v2->y, &tmp);
  lmSub  (&candidate->y, &tmp);
  lmMult (&slope->A, &tmp, &tmp2);

  lmCopy (&v2->x, &tmp);
  lmSub  (&candidate->x, &tmp);
  lmMult (&slope->B, &tmp, &tmp3);

  lmSub (&tmp3, &tmp2);

  lmStore(&lmZero, 0);

  if (lmEq (&tmp2, &lmZero) || lmEq (&tmp4, &lmZero))
    return (candidate->a ==
	    MAX (candidate->a, MAX (v1->a, v2->a)));

  return (lmPositive (&tmp4) != lmPositive (&tmp2));
}

/*---------------------------------------------------------------------------*/

static int
lmComputeAnchor (slope, ov, fv, tv)

     slopeType *slope;
     vertex3dType *ov, *fv, *tv;

{
  if (lmTestIfAnchor (slope, ov, fv, tv)) return ov->a;
  if (lmTestIfAnchor (slope, fv, ov, tv)) return fv->a;

  return tv->a;
}

/*---------------------------------------------------------------------------*/

bool
sosSlopeRightTurn (oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  return sosAngleGT180 (oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz);
}

/*---------------------------------------------------------------------------*/

bool
sosSlopeGT (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z,
	    o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, t2i, t2x, t2y, t2z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z,
               o2x, o2y, o2z, f2x, f2y, f2z, t2x, t2y, t2z;
     indexType o1i, f1i, t1i, o2i, f2i, t2i;

{
  slopeType slopeA, slopeB;
  static vertex3dType o1v, f1v, t1v, o2v, f2v, t2v;
  lmNumberType lmZero;
  indexType anchor;

/*  return sosAngleGT (o1i, o1x, o1y, f1i, f1x, f1y, t1i, t1x, t1y,
		     o2i, o2x, o2y, f2i, f2x, f2y, t2i, t2x, t2y);
*/
/*  printf("Comparing Slope:\n");
  lmPrintSlope (stdout, o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, 
		t1i, t1x, t1y, t1z);
  lmPrintSlope (stdout, o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, 
		t2i, t2x, t2y, t2z);
  printf("\n");
*/

  lmMake3dVertex (&o1v, o1i, o1x, o1y, o1z);
  lmMake3dVertex (&f1v, f1i, f1x, f1y, f1z);
  lmMake3dVertex (&t1v, t1i, t1x, t1y, t1z);
  lmComputeSlope (&o1v, &f1v, &t1v, &slopeA);

  lmMake3dVertex (&o2v, o2i, o2x, o2y, o2z);
  lmMake3dVertex (&f2v, f2i, f2x, f2y, f2z);
  lmMake3dVertex (&t2v, t2i, t2x, t2y, t2z);
  lmComputeSlope (&o2v, &f2v, &t2v, &slopeB);

  if (((slopeA.lexHigh == slopeB.lexMed) &&
       (slopeA.lexMed == slopeB.lexLow) &&
       (slopeA.lexLow == slopeB.lexHigh)) ||
      ((slopeB.lexHigh == slopeA.lexMed) &&
       (slopeB.lexMed == slopeA.lexLow) &&
       (slopeB.lexLow == slopeA.lexHigh))) {
    
    anchor = lmComputeAnchor (&slopeA, &o1v, &f1v, &t1v);
    if (anchor == slopeA.lexHigh) return 1;
    if (anchor == slopeB.lexHigh) return 0;
    if (slopeA.lexHigh > slopeB.lexHigh) return 1;
    if (slopeB.lexHigh > slopeA.lexHigh) return 0;
    return (slopeA.lexMed > slopeB.lexMed);
  } 

  lmStore (&lmZero, 0);

  if (! (lmEq(&slopeA.A2plusB2, &lmZero) && lmEq(&slopeA.C2, &lmZero) &&
	 lmEq(&slopeB.A2plusB2, &lmZero) && lmEq(&slopeB.C2, &lmZero))) {
    if (lmEq(&slopeA.A2plusB2, &lmZero) && lmEq(&slopeA.C2, &lmZero)) 
      return 0;
    if (lmEq(&slopeB.A2plusB2, &lmZero) && lmEq(&slopeB.C2, &lmZero))
      return 1;

    if (lmRatLt (&slopeB.A2plusB2, &slopeB.C2,
		 &slopeA.A2plusB2, &slopeA.C2)) return 1;
    if (lmRatLt (&slopeA.A2plusB2, &slopeA.C2,
		 &slopeB.A2plusB2, &slopeB.C2)) return 0;
    
  }

  if (slopeA.lexHigh > slopeB.lexHigh) return 1;
  if (slopeB.lexHigh > slopeA.lexHigh) return 0;
  if (slopeA.lexMed > slopeB.lexMed) return 1;
  if (slopeB.lexMed > slopeA.lexMed) return 0;
  if (slopeA.lexLow > slopeB.lexLow) return 1;
  if (slopeB.lexLow > slopeA.lexLow) return 0;

  printf("ERROR: Two Slopes have the same size!!??");
  lmPrintSlope (stdout, o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, 
		t1i, t1x, t1y, t1z);
  lmPrintSlope (stdout, o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, 
		t2i, t2x, t2y, t2z);
  return 1;
}

/*---------------------------------------------------------------------------*/

float
sosSlopeFloat (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z;
     indexType o1i, f1i, t1i;

{
  slopeType slopeA;
  static vertex3dType o1v, f1v, t1v;
  lmNumberType lmZero;
  indexType anchor;
  double angle;

  lmMake3dVertex (&o1v, o1i, o1x, o1y, o1z);
  lmMake3dVertex (&f1v, f1i, f1x, f1y, f1z);
  lmMake3dVertex (&t1v, t1i, t1x, t1y, t1z);
  lmComputeSlope (&o1v, &f1v, &t1v, &slopeA);

  lmStore (&lmZero, 0);

  if (! lmEq (&lmZero, &slopeA.C2))
    angle = 180.0 / 3.1415926535897932384626433 * 
      atan (sqrt((double) lmReal (&slopeA.A2plusB2) / lmReal (&slopeA.C2)));
  else
    angle = 90;

  return angle;
}

/*---------------------------------------------------------------------------*/

void
lmPrintSlope (fp, oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     FILE      *fp;
     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  slopeType slope;
  static vertex3dType ov, fv, tv;

  lmMake3dVertex (&ov, oi, ox, oy, oz);
  lmMake3dVertex (&fv, fi, fx, fy, fz);
  lmMake3dVertex (&tv, ti, tx, ty, tz);
  lmComputeSlope (&ov, &fv, &tv, &slope);

  printf("slope (%d, %d, %d) : ", slope.lexHigh, slope.lexMed, slope.lexLow);
  printf("A = "); lmPrint(stdout, &slope.A);
  printf(", B = "); lmPrint(stdout, &slope.B);
  printf(", C = "); lmPrint(stdout, &slope.C);
  printf(", A2 + B2 = "); lmPrint(stdout, &slope.A2plusB2);
  printf(", C2 = "); lmPrint(stdout, &slope.C2);
  printf("\n");
}

/*---------------------------------------------------------------------------*/

lmPrintSlopeFast (fp, slope)

     FILE      *fp;
     slopeType *slope;

{
  printf("slope (%d, %d, %d) : ", slope->lexHigh, slope->lexMed, slope->lexLow);
  printf("A = "); lmPrint(stdout, &slope->A);
  printf(", B = "); lmPrint(stdout, &slope->B);
  printf(", C = "); lmPrint(stdout, &slope->C);
  printf(", A2 + B2 = "); lmPrint(stdout, &slope->A2plusB2);
  printf(", C2 = "); lmPrint(stdout, &slope->C2);
  printf("\n");
}

/*---------------------------------------------------------------------------*/

test () 

{
  int index[6], x[6], y[6], z[6], i;

  do {
    for (i = 0; i <= 6; i++) {
      printf ("point %d (index, x, y, z) ?\n", (i % 3) + 1);
      scanf ("%d %d %d %d", &index[i], &x[i], &y[i], &z[i]);
    }
    
    lmPrintSlope (stdout, index[0], x[0], y[0], z[0],
			 index[1], x[1], y[1], z[1],
			 index[2], x[2], y[2], z[2]);
    lmPrintSlope (stdout, index[3], x[3], y[3], z[3],
			 index[4], x[4], y[4], z[4],
			 index[5], x[5], y[5], z[5]);
    
    printf ("A > B ? %s\n\n", 
	    (sosSlopeGT (index[0], x[0], y[0], z[0],
			 index[1], x[1], y[1], z[1],
			 index[2], x[2], y[2], z[2],
			 index[3], x[3], y[3], z[3],
			 index[4], x[4], y[4], z[4],
			 index[5], x[5], y[5], z[5])
	     ? "YES" : "NO"));
    printf ("B > A ? %s\n\n", 
	    (sosSlopeGT (index[3], x[3], y[3], z[3],
			 index[4], x[4], y[4], z[4],
			 index[5], x[5], y[5], z[5],
	                 index[0], x[0], y[0], z[0],
			 index[1], x[1], y[1], z[1],
			 index[2], x[2], y[2], z[2])
			 
	     ? "YES" : "NO"));
/*    printf ("A < 0 ? %s\n\n", 
	    (sosSlopeGT0 (&slopeA) ? "YES" : "NO"));
    printf ("B < 0 ? %s\n\n", 
	    (sosSlopeGT0 (&slopeB) ? "YES" : "NO"));
*/
    printf ("\n\n\n");
  } while (0);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/



