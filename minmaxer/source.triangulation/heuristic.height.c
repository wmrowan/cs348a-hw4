/*
 * NAME: LongMath.MinMax.c
 *
 * DESCRIPTION:
 *	Contains Longmath functions for the height of a triangle.
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

#define AREA_IS_FINITE 1
#define AREA_IS_MINUS_INFINITE 0

static
lmComputeLexicographicalOrderHeight(vertexType*, vertexType*, vertexType*, heightType*);

/*---------------------------------------------------------------------------*/

lmComputeHeight (originVertex, fromVertex, toVertex, height)
     vertexType  *originVertex, *fromVertex, *toVertex;
     heightType *height;
{
  if (fromVertex->a != toVertex->a) {
    height->status = AREA_IS_FINITE;
    lmComputeArea (originVertex, fromVertex, toVertex,
		   &height->area2);
    lmComputeLength (fromVertex, toVertex, &height->length2);
  } else {
    height->status = AREA_IS_MINUS_INFINITE;
  }

  lmComputeLexicographicalOrderHeight (originVertex, fromVertex, 
				       toVertex, height);
}

/*---------------------------------------------------------------------------*/

lmComputeArea(originVertex, fromVertex, toVertex, area2)

     vertexType *originVertex, *fromVertex, *toVertex;
     lmNumberType *area2;

{
  lmNumberType area;

  (void) lmDet31 (&originVertex->x, &originVertex->y, 
		  &toVertex->x, &toVertex->y,
		  &fromVertex->x, &fromVertex->y,
		  &area);

  lmMult (&area, &area, area2);
  if (! lmPositive (&area))
    lmNegate (area2);
}

/*---------------------------------------------------------------------------*/

lmComputeLength(fromVertex, toVertex, length2)

     vertexType *fromVertex, *toVertex;
     lmNumberType *length2;

{
  lmNumberType tmp0, tmp1;
  
  lmCopy (&toVertex->x, &tmp0);
  lmSub (&fromVertex->x, &tmp0);
  lmMult (&tmp0, &tmp0, length2);
  
  lmCopy (&toVertex->y, &tmp0);
  lmSub (&fromVertex->y, &tmp0);
  lmMult (&tmp0, &tmp0, &tmp1);
  
  lmAdd (&tmp1, length2);
}

/*---------------------------------------------------------------------------*/

static
lmComputeLexicographicalOrderHeight(originVertex, fromVertex, toVertex, height)
     vertexType *originVertex, *fromVertex, *toVertex;
     heightType  *height;
{
  height->lexHigh = originVertex->a;
  height->lexMed = fromVertex->a;
  height->lexLow = toVertex->a;

}

/*---------------------------------------------------------------------------*/

bool
sosHeightLT0 (oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  static lmNumberType lmZero;
  heightType height;
  static vertexType ov, fv, tv;

  lmMakeVertex (&ov, oi, ox, oy);
  lmMakeVertex (&fv, fi, fx, fy);
  lmMakeVertex (&tv, ti, tx, ty);
  lmComputeHeight(&ov, &fv, &tv, &height);

  if ((! lmPositive (&(height.area2))) ||
      (height.status == AREA_IS_MINUS_INFINITE))
    return 1;

  lmStore(&lmZero, 0);
  if(! lmEq(&(height.area2), &lmZero)) return 0;

  if (height.lexMed > height.lexLow) return 1;
  if (height.lexLow > height.lexMed) return 0;

  return 1;
}

/*---------------------------------------------------------------------------*/

bool
sosHeightLT (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z,
	     o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, t2i, t2x, t2y, t2z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z,
               o2x, o2y, o2z, f2x, f2y, f2z, t2x, t2y, t2z;
     indexType o1i, f1i, t1i, o2i, f2i, t2i;

{
  static lmNumberType lmZero;
  heightType heightA, heightB;
  angleType angleA, angleB;
  static vertexType o1v, f1v, t1v, o2v, f2v, t2v;

  lmMakeVertex (&o1v, o1i, o1x, o1y);
  lmMakeVertex (&f1v, f1i, f1x, f1y);
  lmMakeVertex (&t1v, t1i, t1x, t1y);
  lmComputeHeight (&o1v, &f1v, &t1v, &heightA);

  lmMakeVertex (&o2v, o2i, o2x, o2y);
  lmMakeVertex (&f2v, f2i, f2x, f2y);
  lmMakeVertex (&t2v, t2i, t2x, t2y);
  lmComputeHeight (&o2v, &f2v, &t2v, &heightB);

  if ((heightA.status != AREA_IS_MINUS_INFINITE) ||
      (heightB.status != AREA_IS_MINUS_INFINITE)) {
    if (heightA.status == AREA_IS_MINUS_INFINITE) return 1;
    if (heightB.status == AREA_IS_MINUS_INFINITE) return 0;
    if (lmRatLt (&heightB.area2, &heightB.length2,
		 &heightA.area2, &heightA.length2)) return 0;
    if (lmRatLt (&heightA.area2, &heightA.length2,
		 &heightB.area2, &heightB.length2)) return 1;
  }

  lmStore(&lmZero, 0);
  if(lmEq(&(heightA.area2), &lmZero) && lmEq(&(heightB.area2), &lmZero)) {
    lmComputeAngle (&o1v, &f1v, &t1v, &angleA);
    lmComputeAngle (&o2v, &f2v, &t2v, &angleB);

    if ((angleA.quadrant > 0) && (angleB.quadrant == 0)) return 1;
    if ((angleB.quadrant > 0) && (angleA.quadrant == 0)) return 0;
  }

  if (heightA.lexHigh > heightB.lexHigh) return 1;
  if (heightB.lexHigh > heightA.lexHigh) return 0;
  if (heightA.lexMed > heightB.lexMed) return 1;
  if (heightB.lexMed > heightA.lexMed) return 0;
  if (heightA.lexLow > heightB.lexLow) return 1;
  if (heightB.lexLow > heightA.lexLow) return 0;

  printf("ERROR: Two Heights have the same size!!??");
  lmPrintHeight
    (stdout, o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z);
  lmPrintHeight
    (stdout, o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, t2i, t2x, t2y, t2z);

  return 1;
}

/*---------------------------------------------------------------------------*/

float
sosHeightFloat (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z;
     indexType o1i, f1i, t1i;

{
  static lmNumberType lmZero;
  heightType heightA;
  static vertexType o1v, f1v, t1v;
  float height;

  lmMakeVertex (&o1v, o1i, o1x, o1y);
  lmMakeVertex (&f1v, f1i, f1x, f1y);
  lmMakeVertex (&t1v, t1i, t1x, t1y);
  lmComputeHeight (&o1v, &f1v, &t1v, &heightA);

  lmStore(&lmZero, 0);
  if (! lmEq (&lmZero, &heightA.length2)) {
    height = sqrt (fabs (((double) 
		   (lmReal (&heightA.area2) / lmReal (&heightA.length2)))));
    return height;
  } else
    return 0.0;
}

/*---------------------------------------------------------------------------*/

void
lmPrintHeight (fp, oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     FILE      *fp;
     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  heightType height;
  static vertexType ov, fv, tv;

  lmMakeVertex (&ov, oi, ox, oy);
  lmMakeVertex (&fv, fi, fx, fy);
  lmMakeVertex (&tv, ti, tx, ty);
  lmComputeHeight (&ov, &fv, &tv, &height);

  printf("height (%d, %d, %d) : ", height.lexHigh,
	  height.lexMed, height.lexLow);
  if (height.status != AREA_IS_MINUS_INFINITE) {
    printf("4 * area ^ 2 = "); lmPrint(stdout, &height.area2);
    printf(", length ^ 2 = "); lmPrint(stdout, &height.length2); 
    printf("\n");
  } else
    printf("area is minus infinite\n");
}

/*---------------------------------------------------------------------------*/

/*main () 

{
  int index, x, y;
  vertexType v1, v2, v3;
  heightType heightA, heightB;

  do {
    printf ("point 1 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v1, index, x, y);
    
    printf ("point 2 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v2, index, x, y);
    
    printf ("point 3 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v3, index, x, y);
    
    lmComputeHeight (&v3, &v1, &v2, &heightA);
    
    printf ("\n\n\n");
    
    printf ("point 1 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v1, index, x, y);
    
    printf ("point 2 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v2, index, x, y);
    
    printf ("point 3 (index, x, y) ?\n");
    scanf ("%d %d %d", &index, &x, &y);
    lmMakeVertex (&v3, index, x, y);
    
    lmComputeHeight (&v3, &v1, &v2, &heightB);
    
    lmPrintHeight (stdout, &heightA);
    lmPrintHeight (stdout, &heightB);
    
    printf ("A < B ? %s\n\n", 
	    (sosHeightLT (&heightA, &heightB) ? "YES" : "NO"));
    printf ("B < A ? %s\n\n", 
	    (sosHeightLT (&heightB, &heightA) ? "YES" : "NO"));
    printf ("A < 0 ? %s\n\n", 
	    (sosHeightLT0 (&heightA) ? "YES" : "NO"));
    printf ("B < 0 ? %s\n\n", 
	    (sosHeightLT0 (&heightB) ? "YES" : "NO"));

    printf ("\n\n\n");
  } while (0);
}

*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/



