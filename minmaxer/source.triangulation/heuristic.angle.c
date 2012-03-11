/*
 * NAME: LongMath.MinMax.c
 *
 * DESCRIPTION:
 *	Contains Longmath functions for Minimization of Maximum Angle.
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

static
lmComputeRelativeVertexCoordinates(vertexType*, vertexType*, vertexType*);

static
lmRotateFromVertexIntoFirstQuadrantAndAccordinglyRotateToVertex(vertexType*, vertexType*);

static
lmComputeQuadrantOfVertex(vertexType*, int*);

static
lmRotateToVertexIntoFirstQuadrant(vertexType*, vertexType*, vertexType*, vertexType*, angleType*);

static
lmRotateVertex(vertexType*, int);

static
lmComputeDenominator(vertexType*, vertexType*, angleType*);

static
lmComputeNominator(vertexType*, vertexType*, angleType*);

static
lmComputeLexicographicalOrderAngle(vertexType*, vertexType*, vertexType*, angleType*);

/* -------------------- lmComputeAngle -------------------- */

lmComputeAngle(originVertex, fromVertex, toVertex, angle)
     vertexType  *originVertex, *fromVertex, *toVertex;
     angleType *angle;
{
  static vertexType deltaFromVertex[1], deltaToVertex[1];

  lmComputeRelativeVertexCoordinates(originVertex, fromVertex, deltaFromVertex);
  lmComputeRelativeVertexCoordinates(originVertex, toVertex, deltaToVertex);
  lmRotateFromVertexIntoFirstQuadrantAndAccordinglyRotateToVertex(deltaFromVertex, deltaToVertex);
  lmRotateToVertexIntoFirstQuadrant(deltaFromVertex, deltaToVertex,
				    fromVertex, toVertex, angle);
  lmComputeDenominator(deltaFromVertex, deltaToVertex, angle);
  lmComputeNominator(deltaFromVertex, deltaToVertex, angle);
  lmComputeLexicographicalOrderAngle(originVertex, fromVertex, toVertex, angle);
}

/* -------------------- lmComputeRelativeVertexCoordinates -------------------- */

static
lmComputeRelativeVertexCoordinates(vertexA, vertexB, deltaVertex)
     vertexType *vertexA, *vertexB, *deltaVertex;
{
  lmCopy(&vertexB->x, &deltaVertex->x);  
  lmSub(&vertexA->x, &deltaVertex->x);
  lmCopy(&vertexB->y, &deltaVertex->y);  
  lmSub(&vertexA->y, &deltaVertex->y);

}

/* ------lmRotateFromVertexIntoFirstQuadrantAndAccordinglyRotateToVertex ------ */

static
lmRotateFromVertexIntoFirstQuadrantAndAccordinglyRotateToVertex(fromVertex, toVertex)
     vertexType *fromVertex, *toVertex;
{
  static int quadrant;

  lmComputeQuadrantOfVertex(fromVertex, &quadrant);
  lmRotateVertex(fromVertex, quadrant);
  lmRotateVertex(toVertex, quadrant);

/*  printf("\nafter first rotation from quadrant %d:\n", quadrant);
  lmPrintVertex(fromVertex);
  lmPrintVertex(toVertex);*/

}

/* ------ lmRotateToVertexIntoFirstQuadrant ------ */

static
lmRotateToVertexIntoFirstQuadrant(fromVertex, toVertex, fromIndex,
				  toIndex, angle)
     vertexType  *fromVertex, *toVertex, *fromIndex, *toIndex;
     angleType *angle;
{
  static int quadrant;

  lmComputeQuadrantOfVertex(toVertex, &quadrant);
  lmRotateVertex(toVertex, quadrant);

  if (fromIndex->a == toIndex->a)
    angle->quadrant = 4;
  else
    if (lmRatLt(&fromVertex->y, &fromVertex->x,
		&toVertex->y, &toVertex->x)) {
      lmRotateVertex(toVertex, 3);
      angle->quadrant = (quadrant + 3) % 4;
    }
    else
      angle->quadrant = quadrant;

  /*  printf("\nafter second rotation:\n");
      lmPrintVertex(fromVertex);
      lmPrintVertex(toVertex);*/
}

/* -------------------- lmComputeQuadrantOfVertex -------------------- */

static
lmComputeQuadrantOfVertex(vertexA, quadrant)
     vertexType  *vertexA;
     int       *quadrant;
{
  static lmNumberType lmZero;

  lmStore(&lmZero, 0);

  if (lmPositive(&vertexA->x)) 
    if (lmPositive(&vertexA->y))
      if (lmEq(&vertexA->x, &lmZero))
	*quadrant = 3;
      else
	*quadrant = 0;
    else 
      *quadrant = 1;
  else 
    if (lmPositive(&vertexA->y)) 
      if (lmEq(&vertexA->y, &lmZero))
	*quadrant = 2;
      else
	*quadrant = 3;
    else 
      *quadrant = 2;

}

/* -------------------- lmRotateVertex -------------------- */

static
lmRotateVertex(vertexA, NumberOfRotations)
     vertexType  *vertexA;
     int       NumberOfRotations;
{
  static lmNumberType lmZero;

  lmStore(&lmZero, 0);

  if (NumberOfRotations > 0) {
    if ((NumberOfRotations % 2) == 1) lmExchange(&vertexA->x, &vertexA->y);
    if (NumberOfRotations <= 2) 
      if(! lmEq(&vertexA->x, &lmZero))
	lmNegate(&vertexA->x);
    if (NumberOfRotations >= 2) 
      if(! lmEq(&vertexA->y, &lmZero))
	lmNegate(&vertexA->y);

  }

}

/* -------------------- lmComputeNominator -------------------- */

static
lmComputeNominator(vertexA, vertexB, angle)
     vertexType  *vertexA, *vertexB;
     angleType *angle;
{
  static lmNumberType xAyB[1], xByA[1];

  lmMult(&vertexB->x, &vertexA->y, xByA);
  lmMult(&vertexA->x, &vertexB->y, xAyB);  
  lmCopy(xByA, &angle->nominator);
  lmSub(xAyB, &angle->nominator);

}

/* -------------------- lmComputeDenominator -------------------- */

static
lmComputeDenominator(vertexA, vertexB, angle)
     vertexType *vertexA, *vertexB;
     angleType *angle;
{
  static lmNumberType xAxB[1], yAyB[1];

  lmMult(&vertexA->x, &vertexB->x, xAxB);  
  lmMult(&vertexA->y, &vertexB->y, yAyB);
  lmCopy(yAyB, &angle->denominator);
  lmAdd(xAxB, &angle->denominator);

}

/* -------------------- lmComputeLexicographicalOrder -------------------- */

static
lmComputeLexicographicalOrderAngle(originVertex, fromVertex, toVertex, angle)
     vertexType *originVertex, *fromVertex, *toVertex;
     angleType  *angle;
{
  angle->lexHigh = originVertex->a;
  angle->lexMed = fromVertex->a;
  angle->lexLow = toVertex->a;

}

/* -------------------- angleGreaterThan 180 -------------------- */

bool
sosAngleGT180 (oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  static lmNumberType lmZero;
  static angleType angle;
  static vertexType ov, fv, tv;

  lmMakeVertex (&ov, oi, ox, oy);
  lmMakeVertex (&fv, fi, fx, fy);
  lmMakeVertex (&tv, ti, tx, ty);
  lmComputeAngle(&ov, &fv, &tv, &angle);

  if (angle.quadrant > 2) return 1;
  if (angle.quadrant < 2) return 0;
  lmStore(&lmZero, 0);
  if(! lmEq(&(angle.nominator), &lmZero)) return 1;
  if (angle.lexMed > angle.lexLow) return 1;
  if (angle.lexLow > angle.lexMed) return 0;

  return 0;
}

/* -------------------- angleGreaterThan -------------------- */

bool
sosAngleGT (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z,
	    o2i, o2x, o2y, o2z, f2i, f2x, f2y, f2z, t2i, t2x, t2y, t2z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z,
               o2x, o2y, o2z, f2x, f2y, f2z, t2x, t2y, t2z;
     indexType o1i, f1i, t1i, o2i, f2i, t2i;

{
  static angleType angleA, angleB;
  static vertexType o1v, f1v, t1v, o2v, f2v, t2v;

  lmMakeVertex (&o1v, o1i, o1x, o1y);
  lmMakeVertex (&f1v, f1i, f1x, f1y);
  lmMakeVertex (&t1v, t1i, t1x, t1y);
  lmComputeAngle(&o1v, &f1v, &t1v, &angleA);

  lmMakeVertex (&o2v, o2i, o2x, o2y);
  lmMakeVertex (&f2v, f2i, f2x, f2y);
  lmMakeVertex (&t2v, t2i, t2x, t2y);
  lmComputeAngle(&o2v, &f2v, &t2v, &angleB);

  if (angleA.quadrant > angleB.quadrant) return 1;
  if (angleA.quadrant < angleB.quadrant) return 0;
  if (lmRatLt(&angleB.nominator, &angleB.denominator,
	      &angleA.nominator, &angleA.denominator)) return 1;
  if (lmRatLt(&angleA.nominator, &angleA.denominator,
	      &angleB.nominator, &angleB.denominator)) return 0;
  if (angleA.lexHigh > angleB.lexHigh) return 1;
  if (angleB.lexHigh > angleA.lexHigh) return 0;
  if (angleA.lexMed > angleB.lexMed) return 1;
  if (angleB.lexMed > angleA.lexMed) return 0;
  if (angleA.lexLow > angleB.lexLow) return 1;
  if (angleB.lexLow > angleA.lexLow) return 0;

  printf("ERROR: Two Angles have the same size!!??");
  lmPrintAngle(stdout, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z);
  lmPrintAngle(stdout, o2x, o2y, o2z, f2i, f2x, f2y, f2z, t2i, t2x, t2y, t2z);

  return 0;
}

/* -------------------- Angle Ratio -------------------- */

float
sosAngleFloat (o1i, o1x, o1y, o1z, f1i, f1x, f1y, f1z, t1i, t1x, t1y, t1z)

     coordType o1x, o1y, o1z, f1x, f1y, f1z, t1x, t1y, t1z;
     indexType o1i, f1i, t1i;

{
  static lmNumberType lmZero;
  static angleType angleA;
  static vertexType o1v, f1v, t1v, o2v, f2v, t2v;
  double angle;

  lmMakeVertex (&o1v, o1i, o1x, o1y);
  lmMakeVertex (&f1v, f1i, f1x, f1y);
  lmMakeVertex (&t1v, t1i, t1x, t1y);
  lmComputeAngle(&o1v, &f1v, &t1v, &angleA);

  angle = angleA.quadrant * 90;
  lmStore(&lmZero, 0);
  if (! lmEq (&lmZero, &angleA.denominator)) {
    angle = angle + 180.0 / 3.1415926535897932384626433 *
            atan (lmReal (&angleA.nominator) / lmReal (&angleA.denominator));
  }

  return (float) angle;
}

/* -------------------- lmPrintAngle -------------------- */

void
lmPrintAngle (fp, oi, ox, oy, oz, fi, fx, fy, fz, ti, tx, ty, tz)

     FILE      *fp;
     coordType ox, oy, oz, fx, fy, fz, tx, ty, tz;
     indexType oi, fi, ti;

{
  static angleType angle;
  static vertexType ov, fv, tv;

  lmMakeVertex (&ov, oi, ox, oy);
  lmMakeVertex (&fv, fi, fx, fy);
  lmMakeVertex (&tv, ti, tx, ty);
  lmComputeAngle(&ov, &fv, &tv, &angle);

  printf("angle (%d, %d, %d) = ", angle.lexHigh,
	  angle.lexMed, angle.lexLow);
  printf("(quadrant = %d, ", angle.quadrant); 
  printf("num = "); lmPrint(stdout, &angle.nominator);
  printf(", denum = "); lmPrint(stdout, &angle.denominator); 
  printf("\n");
}

/* -------------------- lmMakeVertex  -------------------- */

lmMakeVertex(vertex, index, xCoord, yCoord)
     vertexType *vertex;
     indexType index;
     coordType xCoord, yCoord;
{
  lmStore(&vertex->x, xCoord);
  lmStore(&vertex->y, yCoord);
  vertex->a = index;
}

/* -------------------- lmPrintVertex  -------------------- */

lmPrintVertex(vertex)
     vertexType *vertex;
{
  printf("Vertex %d = (", vertex->a); lmPrint(stdout, &vertex->x);
  printf(", "); lmPrint(stdout, &vertex->y); printf(")\n");
}

/* --------------------   -------------------- */

static
dummy()

{

}

