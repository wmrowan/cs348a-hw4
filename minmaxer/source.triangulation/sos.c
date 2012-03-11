/*
 * NAME: sos.c
 *
 * DESCRIPTION:
 *	This file contains routines supporting "Simulation of Simplicity,"
 *	SoS for short.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/11/29: Ability to display which perturbation case used added
 *	89/11/30: Verification routines added
 *	89/12/05: left turn and incircle changed to order points
 *	89/12/06: non-zero soscase's only shown with sosDebug
 *	89/12/06: sosStats added
 *	90/02/28: leftTurn and incircle change sign of detdbl based on nswaps
 *	90/11/11: leftTurn sort bug fixed
 *
 * SEE ALSO:
 *	H. Edelsbrunner and E. Mucke, "Simulation of Simplicity: A
 *	Technique to Cope with Degenerate Cases in Geometric Algorithms,"
 *	UIUC Technical Report No. UIUCDCS-R-87-1393, December 1987.
 */
#include <stdlib.h>
#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"

#define EPSILON 0.001
#define ABS(x) ((x) > 0.0 ? (x) : -(x))
#define X2(x) ((x) * (x))

int sosDebug = 0;

int leftTurnPerts = 0, incirclePerts = 0;

int leftTurnMath(x1, y1, x2, y2, x3, y3, detdbl)
coordType x1, y1, x2, y2, x3, y3;
double *detdbl;
/* Returns:
 *           | x1  y1  1 |
 *           | x2  y2  1 | > 0
 *           | x3  y3  1 |
 */
{
  int det, soscase;
  static lmNumberType r[7];

  /* load long math registers */
  lmStore(&r[1], x1);
  lmStore(&r[2], y1);
  lmStore(&r[3], x2);
  lmStore(&r[4], y2);
  lmStore(&r[5], x3);
  lmStore(&r[6], y3);

  if (det = lmDet31(&r[1],&r[2],&r[3],&r[4],&r[5],&r[6], &r[0]))
    soscase = 0;
  else if (det = x3 - x2)
    soscase = 1;
  else if (det = y2 - y3)
    soscase = 2;
  else if (det = x1 - x3)
    soscase = 3;
  else {
    det = 1;
    soscase = 4;
  }

  if (sosDebug) {
    (void) printf("\nleftTurnMath: (%d, %d) (%d, %d) (%d, %d)\n",
      x1, y1, x2, y2, x3, y3);
  }

  if (sosDebug && soscase)
    (void) printf("leftTurnMath: pertubation soscase %d.\n", soscase);

  if (soscase == 0)
    *detdbl = lmReal(&r[0]);
  else {
    *detdbl = 0.0;
    leftTurnPerts++;
  }

  return (det > 0);
}  /* -------------------- leftTurnMath -------------------- */


int 
leftTurnCoords (x0, y0, s0, x1, y1, s1, x2, y2, s2, detdbl)

     indexType s0, s1, s2;
     coordType x1, y1, x2, y2, x0, y0;
     double *detdbl;

{
  int result, nswaps = 0;
  indexType tmp, s[3];
  coordType x[3], y[3], tmpC;

  s[0] = s0;  s[1] = s1;  s[2] = s2;
  x[0] = x0;  x[1] = x1;  x[2] = x2;
  y[0] = y0;  y[1] = y1;  y[2] = y2;
  
  if (s[0] > s[1]) {
    tmp = s[0];  s[0] = s[1];  s[1] = tmp;
    tmpC = x[0];  x[0] = x[1];  x[1] = tmpC;
    tmpC = y[0];  y[0] = y[1];  y[1] = tmpC;
    nswaps++;
  }

  if (s[1] > s[2]) {
    tmp = s[1];  s[1] = s[2];  s[2] = tmp;
    tmpC = x[1];  x[1] = x[2];  x[2] = tmpC;
    tmpC = y[1];  y[1] = y[2];  y[2] = tmpC;
    nswaps++;

    if (s[0] > s[1]) {
      tmp = s[0];  s[0] = s[1];  s[1] = tmp;
      tmpC = x[0];  x[0] = x[1];  x[1] = tmpC;
      tmpC = y[0];  y[0] = y[1];  y[1] = tmpC;
      nswaps++;
    }
  }

  result = leftTurnMath(x[0], y[0], x[1], y[1], x[2], y[2], detdbl);

  if (sosDebug) {
    (void) printf("leftTurn: %d %d %d: math result %d, swaps: %d,", 
      s0, s1, s2, result, nswaps);
  }

  if (nswaps & 1) {
    result = ! result;
    *detdbl = -(*detdbl);
  }

  if (sosDebug) {
    (void) printf(" answer: %d\n", result);
  }

  return result;
}


int 
leftTurn(g, s0, s1, s2, detdbl)

     graphType *g;
     indexType s0, s1, s2;
     double *detdbl;

{
  return
    leftTurnCoords ((coordType) SITEX(g, s0), (coordType) SITEY(g, s0), 
		    (indexType) SITEN (g, s0), 
		    (coordType) SITEX(g, s1), (coordType) SITEY(g, s1), 
		    (indexType) SITEN (g, s1), 
		    (coordType) SITEX(g, s2), (coordType) SITEY(g, s2), 
		    (indexType) SITEN (g, s2), 
		    detdbl);
}

int regularityMath(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, detdbl)
coordType x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
double *detdbl;
/* Returns:				    Registers:
 *    | x1  y1  (x1^2 + y1^2 - z1)  1 |		| 1  2  3  x |
 *    | x2  y2  (x2^2 + y2^2 - z2)  1 |		| 4  5  6  x |
 *    | x3  y3  (x3^2 + y3^2 - z3)  1 | > 0	| 7  8  9  x |
 *    | x4  y4  (x4^2 + y4^2 - z4)  1 |		| 10 11 12 x |
 *
 *   s1, s2, s3 and s4 assumed to be in counter-clockwise order in
 * a circle.
 */
{
  int det, soscase;
  static lmNumberType r[13];

  /* set up values in the long math registers */
  lmStore(&r[1], x1);
  lmStore(&r[2], y1);
  lmMult(&r[1], &r[1], &r[3]);
  lmMult(&r[2], &r[2], &r[0]);
  lmAdd(&r[0], &r[3]);			/* r3 = x1^2 + y1^2 */
  lmStore(&r[0], z1);
  lmSub(&r[0], &r[3]);			/* r3 = x1^2 + y1^2 - z1*/


  lmStore(&r[4], x2);
  lmStore(&r[5], y2);
  lmMult(&r[4], &r[4], &r[6]);
  lmMult(&r[5], &r[5], &r[0]);
  lmAdd(&r[0], &r[6]);			/* r6 = x2^2 + y2^2 */
  lmStore(&r[0], z2);
  lmSub(&r[0], &r[6]);			/* r6 = x2^2 + y2^2 - z2*/

  lmStore(&r[7], x3);
  lmStore(&r[8], y3);
  lmMult(&r[7], &r[7], &r[9]);
  lmMult(&r[8], &r[8], &r[0]);
  lmAdd(&r[0], &r[9]);			/* r9 = x3^2 + y3^2 */
  lmStore(&r[0], z3);
  lmSub(&r[0], &r[9]);			/* r9 = x3^2 + y3^2 - z3*/

  lmStore(&r[10], x4);
  lmStore(&r[11], y4);
  lmMult(&r[10], &r[10], &r[12]);
  lmMult(&r[11], &r[11], &r[0]);
  lmAdd(&r[0], &r[12]);			/* r12 = x4^2 + y4^2 */
  lmStore(&r[0], z4);
  lmSub(&r[0], &r[12]);			/* r12 = x4^2 + y4^2 - z4 */

  if (det = lmDet41(&r[1],&r[2],&r[3],&r[4],&r[5],
              &r[6],&r[7],&r[8],&r[9],&r[10],&r[11],&r[12], &r[0])) 
    soscase = 0;
  else if (det =   lmDet31(&r[4],&r[5],&r[7],&r[8],&r[10],&r[11], &r[0]))
    soscase = 1;
/*  else if (det = - lmDet31(&r[1],&r[3],&r[7],&r[9],&r[10],&r[12], &r[0]))*/
  else if (det = - lmDet31(&r[4],&r[6],&r[7],&r[9],&r[10],&r[12], &r[0]))
    soscase = 2;
  else if (det =   lmDet31(&r[5],&r[6],&r[8],&r[9],&r[11],&r[12], &r[0]))
    soscase = 3;
  else if (det = - lmDet31(&r[1],&r[2],&r[7],&r[8],&r[10],&r[11], &r[0]))
    soscase = 4;
  else if (det =   x3 - x4)
    soscase = 5;
  else if (det =   y4 - y3)
    soscase = 6;
  else if (det =   lmDet31(&r[1],&r[3],&r[7],&r[9],&r[10],&r[12], &r[0]))
    soscase = 7;
  else if (det =   lmLt(&r[9], &r[12]))
    soscase = 8;
  else if (det = - lmDet31(&r[2],&r[3],&r[8],&r[9],&r[11],&r[12], &r[0]))
    soscase = 9;
  else if (det =   lmDet31(&r[1],&r[2],&r[4],&r[5],&r[10],&r[11], &r[0])) 
    soscase = 10;
  else if (det =   x4 - x2)
    soscase = 11;
  else if (det =   y2 - y4)
    soscase = 12;
  else if (det =   x1 - x4)
    soscase = 13;
  else {
    det = 1;
    soscase = 14;
  }

  if (soscase == 0)
    *detdbl = lmReal(&r[0]);
  else
    *detdbl = 0.0;

  if (sosDebug) {
    (void) printf("\nregularityMath: (%d, %d) (%d, %d) (%d, %d) (%d, %d) ",
      x1, y1, x2, y2, x3, y3, x4, y4);
    (void) printf("math result %d\n", (det > 0));
  }

  if (sosDebug && soscase)
    (void) printf("regularityMath: perturbation soscase %d.\n", soscase);

  if (soscase)
    incirclePerts++;

  return(det > 0);
}  /* -------------------- regularityMath -------------------- */


int sosRegularTest (g, s0, s1, s2, s3, doDelaunay, detdbl)
graphType *g;
int s0, s1, s2, s3;
int doDelaunay;
double *detdbl;
{
  static int site[4];
  int i, j, tmp, result, nswaps = 0;

  site[0] = s0; site[1] = s1; site[2] = s2; site[3] = s3;

  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (SITEN (g, site[j]) > SITEN (g, site[j+1])) {
	tmp = site[j]; site[j] = site[j+1]; site[j+1] = tmp;
	nswaps++;
      }
    }
  }

/*  printf("nswaps %d %d %d %d %d\n", s0, s1, s2, s3, nswaps);*/

  if (doDelaunay)
    result = regularityMath(SITEX(g, site[0]), SITEY(g, site[0]), (coordType) 0,
			    SITEX(g, site[1]), SITEY(g, site[1]), (coordType) 0,
			    SITEX(g, site[2]), SITEY(g, site[2]), (coordType) 0,
			    SITEX(g, site[3]), SITEY(g, site[3]), (coordType) 0,
			    detdbl);
  else
    result = regularityMath(SITEX(g, site[0]), SITEY(g, site[0]), 
			    SITEZ(g, site[0]),
			    SITEX(g, site[1]), SITEY(g, site[1]),
			    SITEZ(g, site[1]),
			    SITEX(g, site[2]), SITEY(g, site[2]),
			    SITEZ(g, site[2]),
			    SITEX(g, site[3]), SITEY(g, site[3]), 
			    SITEZ(g, site[3]),
			    detdbl);

  if (nswaps & 1) {
    result = ! result;
    *detdbl = - (*detdbl);
  }

  if (sosDebug) {
    (void) printf("incircle: %d %d %d %d (%d swaps) ", s0,s1,s2,s3,nswaps);
    (void) printf(" answer: %d\n", result);
  }

  return result;
}  /* ------------------- sosRegularTest -------------------- */

int incircle (g, s0, s1, s2, s3, detdbl)
graphType *g;
int s0, s1, s2, s3;
double *detdbl;
{
  return sosRegularTest (g, s0, s1, s2, s3, 1, detdbl);
}

int ic1(g, s0, s2, s3)		/* incircle1 */
graphType *g;
indexType s0, s2, s3;
/*
 *   This routine implements an incircle predicate, where site zero
 * and site two are assumed to be opposite points on the circle (i.e.
 * the diameter).  True is returned if point three is within the circle
 * defined by the first two points.
 *   This routine obtains an answer by calculating another site (s1)
 * which is on the circle which has the line s0 to s2 as its diameter.
 * The calculated site could have results which would be fractional
 * (with a half, e.g. 10.5).  In order to avoid this, each of the three
 * other site values are doubled, which does not change the determinate,
 * but it prevents the half values.
 */
{
  coordType x[4], y[4];
  coordType originX, originY, x1Origin, y1Origin, tmp;
  int i, j, result, nswaps = 0;
  double dontcare;

  x[0] = SITEX(g, s0); y[0] = SITEY(g, s0);
  x[2] = SITEX(g, s2); y[2] = SITEY(g, s2);
  x[3] = SITEX(g, s3); y[3] = SITEY(g, s3);
  x[1] = 0; y[1] = 0;				/* temporary */

  /* double coordinate values to avoid half values for site 2 */
  for (i=0; i < 4; i++) {
    x[i] += x[i];  y[i] += y[i];
  }

  /* figure origin of circle */
  originX = (x[0] + x[2]) / 2;
  originY = (y[0] + y[2]) / 2;

  /* figure coordinates of site zero with respect to origin */
  x1Origin = x[0] - originX;
  y1Origin = y[0] - originY;

  /* figure site 1 */
  x[1] = -y1Origin + originX;
  y[1] =  x1Origin + originY;

  /* insertion sort sites */
  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if ((x[j] > x[j+1])  ||  (x[j] == x[j+1] && y[j] > y[j+1])) {
	tmp = x[j]; x[j] = x[j+1]; x[j+1] = tmp;
	tmp = y[j]; y[j] = y[j+1]; y[j+1] = tmp;
	nswaps++;
      }
    }
  }

  result = regularityMath(x[0], y[0], (coordType) 0,
			  x[1], y[1], (coordType) 0,
			  x[2], y[2], (coordType) 0, 
			  x[3], y[3], (coordType) 0, 
			  &dontcare);

  if (nswaps & 1) result = ! result;

  if (sosDebug) {
    (void) printf("ic1: nswaps: %d, answer: %d\n", nswaps, result);
  }

  return result;
}  /* -------------------- ic1 -------------------- */


void leftTurnVerify()
{
  int gt0, i;
  int ntests = 9;
  double dontcare;
  static int t[9][7] = {
    {  2,  3,  5,  7,  11,  13,	 0 },	/* case 0 */
    { -2,  3, -5,  7, -11,  13,	 1 },
    { -2,  3, -2,  3,  -5,   7,	 0 },	/* case 1 */
    {  2,  3,  2,  3,   5,   7,	 1 },
    {  2,  3,  2,  3,   2,   5,	 0 },	/* case 2 */
    {  2, -3,  2, -3,   2,  -5,  1 },
    {  2,  3,  5,  7,   5,   7,  0 },	/* case 3 */
    { -2,  3, -5,  7,  -5,   7,  1 },
    {  2,  3,  2,  5,   2,   5,  1 }	/* case 4 */
  };

  for (i=0; i < ntests; i++) {
    gt0 = leftTurnMath(
      t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5], &dontcare);
    if ((gt0 && !t[i][6]) || (!gt0 && t[i][6])) {
      (void) printf("leftTurnVerify: fail for test case %d\n", i);
      (void) printf(
        "  leftTurn(%d, %d, %d, %d, %d, %d) ==> expected %d, got %d\n",
        t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5], t[i][6], gt0);
    }
  }
}  /* ---------------------- leftTurnVerify -------------------- */


/*   incircleVerify runs test cases through incircleMath.  Not all
 * perturbation cases can be tested since incircleMath generates the
 * third column of numbers on its own (z = x^2 + y^2).  Thus one
 * cannot create a test case which will be zero for the proceeding
 * perturbations without being always being zero for some of the
 * later cases.
 */
incircleVerify()
{
  double dontcare;
  int gt0, i;
  int ntests = 19;
  static int t[19][9] = {
    {  1,  2,  2,  3,  3,  5,  5,  7,	0 },	/* case 0 */
    { -1,  2, -2,  3, -3,  5, -5,  7,   1 },
    {  1,  2,  1,  2,  3,  5,  5,  7,   0 },	/* case 1 */
    { -1,  2, -1,  2, -3,  5, -5,  7,   1 },

    { -2,  3, -1,  2, -1,  2, -3,  5,	0 },	/* case 4 */
    {  2,  3,  1,  2,  1,  2,  3,  5,	1 },
    {  1,  2,  1,  2,  1,  2,  3,  5,   0 },	/* case 5 */
    { -1,  2, -1,  2, -1,  2, -3,  5,	1 },
    {  1, -2,  1, -2,  1, -2,  1, -3,	0 },	/* case 6 */
    {  1,  2,  1,  2,  1,  2,  1,  3,	1 },

    { -1,  2, -2,  3, -3,  5, -3,  5,	0 },	/* case 10 */
    {  1,  2,  2,  3,  3,  5,  3,  5,	1 },
    { -1,  2, -1,  2, -2,  3, -2,  3,	0 },	/* case 11 */
    {  1,  2,  1,  2,  2,  3,  2,  3,	1 },
    {  2, -3,  2, -5,  2, -3,  2, -3,	0 },	/* case 12 */
    {  2,  3,  2,  5,  2,  3,  2,  3,	1 },
    { -3,  3, -2,  3, -2,  3, -2,  3,	0 },	/* case 13 */
    {  3,  3,  2,  3,  2,  3,  2,  3,	1 },
    {  2,  3,  2,  3,  2,  3,  2,  3,	1 }	/* case 14 */
  };

  for (i=0; i < ntests; i++) {
    gt0 = regularityMath(
      t[i][0], t[i][1], 0, t[i][2], t[i][3], 0, t[i][4], t[i][5], 0, 
		       t[i][6], t[i][7], 0,
      &dontcare);
    if ((gt0 && !t[i][8]) || (!gt0 && t[i][8])) {
      (void) printf("incircleVerify: fail for test case %d\n", i);
      (void) printf(
        "  incircle(%d, %d, %d, %d, %d, %d, %d, %d) ==> expected %d, got %d\n",
        t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5], t[i][6], t[i][7],
        t[i][8], gt0);
    }
  }
}  /* -------------------- incircleVerify --------------------- */


void sosStats()
{
  (void) printf("sosStats: degeneracies handled: leftTurn: %d, incircle: %d\n",
    leftTurnPerts, incirclePerts);
}  /* -------------------- sosStats --------------------- */
  

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void
copyFinitePoints (g, index, p, x, y)

     graphType *g;
     int index;
     indexType p;
     coordType *x, *y;

{
  x[index] = SITEX (g, p);
  y[index] = SITEY (g, p);
}
  
/*---------------------------------------------------------------------------*/

static void
copyInfinitePoints (g, index, p, x, y)

     graphType *g;
     int index;
     indexType p;
     coordType *x, *y;

{
  x[index] = g->infP[-p][0];
  y[index] = g->infP[-p][1];
}

/*---------------------------------------------------------------------------*/

static void
copyFinitePoints2 (g, index, p, x, y, z, doDelaunay)

     graphType *g;
     int index;
     indexType p;
     coordType *x, *y, *z;
     int doDelaunay;

{
  x[index] = SITEX (g, p);
  y[index] = SITEY (g, p);
  if (doDelaunay)
    z[index] = 0;
  else
    z[index] = SITEZ (g, p);
}
  
/*---------------------------------------------------------------------------*/

static void
copyInfinitePoints2 (g, index, p, x, y, z, doDelaunay)

     graphType *g;
     int index;
     indexType p;
     coordType *x, *y, *z;
     int doDelaunay;

{
  x[index] = g->infP[-p][0];
  y[index] = g->infP[-p][1];
  z[index] = g->infP[-p][0] * g->infP[-p][0] + g->infP[-p][1] * g->infP[-p][1];
}

/*---------------------------------------------------------------------------*/

static int
inCircle3Inf (x, y, z)

     coordType x[4], y[4], z[4];

{
  int result;

  result = (z[0] * (x[1] * y[2] - y[1] * x[2]) -
	    z[1] * (x[0] * y[2] - y[0] * x[2]) +
	    z[2] * (x[0] * y[1] - y[0] * x[1]));

  if (result == 0) {
    printf("ERROR: incircleInf: 3 infinite points: result == 0 !\n");
    exit (1);
  }

  return result > 0;
}

/*---------------------------------------------------------------------------*/

static int
inCircle2Inf (x, y, z)

     coordType x[4], y[4], z[4];

{
  int result;
  
  lmNumberType ll1, ll2, ly2, lx2, ly3, lx3, p1, p2, d1, d2;

  lmStore (&p1, z[0]);
  lmStore (&p2, x[1]);
  lmMult (&p1, &p2, &ll1);
  lmStore (&p1, z[1]);
  lmStore (&p2, x[0]);
  lmMult (&p1, &p2, &d1);
  lmSub (&d1, &ll1);
  
  lmStore (&p1, z[0]);
  lmStore (&p2, y[1]);
  lmMult (&p1, &p2, &ll2);
  lmStore (&p1, z[1]);
  lmStore (&p2, y[0]);
  lmMult (&p1, &p2, &d1);
  lmSub (&d1, &ll2);
  
  lmStore (&ly2, y[2]);        lmStore (&lx2, x[2]);    
  lmStore (&ly3, y[3]);        lmStore (&lx3, x[3]);    
  lmDet21 (&ly2, &ly3, &d1);   lmDet21 (&lx2, &lx3, &d2); 
  lmMult (&ll1, &d1, &p1);     lmMult (&ll2, &d2, &p2);  

  lmSub (&p2, &p1);

  if (! (result = lmSign (&p1))) 
    if (! (result = lmSign (&ll1)))
      if (! (result = - lmSign (&ll2))) {
	printf("ERROR: incircleInf: 2 infinite points: result == 0 !\n");
	exit (1);
      }
  
  return result > 0;
}
	
/*---------------------------------------------------------------------------*/

static int
inCircle1Inf (x, y, z)

     coordType x[4], y[4], z[4];

{
  int result;
  double detdbl;

  result = leftTurnMath (x[1], y[1], x[2], y[2], x[3], y[3], &detdbl);

  return result > 0;
}

/*---------------------------------------------------------------------------*/

int 
inCircleInf (g, s0, s1, s2, s3, doDelaunay, detdbl)

     graphType *g;
     int s0, s1, s2, s3;
     int doDelaunay;
     double *detdbl;

{
  static int site[4];
  int i, j, tmp, result, nswaps = 0;
  coordType x[4], y[4], z[4];

  if ((s0 >= 0) && (s1 >= 0) && (s2 >= 0) && (s3 >= 0)) 
    /* all vertices are finite vertices */

    return sosRegularTest (g, s0, s1, s2, s3, doDelaunay, detdbl);
  
  site[0] = s0; site[1] = s1; site[2] = s2; site[3] = s3;

  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (SITEN(g, site[j]) > SITEN (g, site[j+1])) {
	tmp = site[j]; site[j] = site[j+1]; site[j+1] = tmp;
	nswaps++;
      }
    }
  }

  if (site[0] >= 0) copyFinitePoints2 (g, 0, site[0], x, y, z, doDelaunay);
  else copyInfinitePoints2 (g, 0, site[0], x, y, z, doDelaunay);
  if (site[1] >= 0) copyFinitePoints2 (g, 1, site[1], x, y, z, doDelaunay);
  else copyInfinitePoints2 (g, 1, site[1], x, y, z, doDelaunay);
  if (site[2] >= 0) copyFinitePoints2 (g, 2, site[2], x, y, z, doDelaunay);
  else copyInfinitePoints2 (g, 2, site[2], x, y, z, doDelaunay);
  if (site[3] >= 0) copyFinitePoints2 (g, 3, site[3], x, y, z, doDelaunay);
  else copyInfinitePoints2 (g, 3, site[3], x, y, z, doDelaunay);

  /* now the infinite points are on top of the list */

  if (site[2] < 0) 
    /* first case: 3 infinite points */
    result = inCircle3Inf (x, y, z);
  else
    if (site[1] < 0)
      /* second case: 2 infinite points */
      result = inCircle2Inf (x, y, z);
    else
      /* third case: 1 infinite point */
      result = inCircle1Inf (x, y, z);

  if (nswaps & 1) {
    result = ! result;
    *detdbl = - (*detdbl);
  }

  if (sosDebug) {
    (void) printf("incircle: %d %d %d %d (%d swaps) ", s0,s1,s2,s3,nswaps);
    (void) printf(" answer: %d\n", result);
  }

  *detdbl = 0;
  return result;
}

/*---------------------------------------------------------------------------*/

static int
leftTurn3Inf (x, y)

     coordType x[3], y[3];

{
  int result;

  result = x[0] * (y[1] - y[2]) - 
           y[0] * (x[1] - x[2]) +
	   (x[1] * y[2] - x[2] * y[1]);

  if (result == 0){
    printf("leftTurnInf: 3 infinite points: result == 0 !\n");
    exit (1);
  }

  return result > 0;
}

/*---------------------------------------------------------------------------*/

static int
leftTurn2Inf (x, y)

     coordType x[3], y[3];

{
  int result;

  if (! (result = x[0] * y[1] - x[1] * y[0]))
    if (! (result = - (x[0] - x[1])))
      if (! (result = (y[0] - y[1]))) {
	printf("leftTurnInf: 3 infinite points: result == 0 !\n");
	exit (1);
      }
  
  return result > 0;
}

/*---------------------------------------------------------------------------*/

static int
leftTurn1Inf (x, y)

     coordType x[3], y[3];

{
  int result;
  lmNumberType k1, k2, ly2, lx2, ly1, lx1, p1, p2, d1, d2;
  
  lmStore (&k1, x[0]);         lmStore (&k2, y[0]);

  lmStore (&ly1, y[1]);        lmStore (&lx1, x[1]);
  lmStore (&ly2, y[2]);        lmStore (&lx2, x[2]);
  lmDet21 (&ly1, &ly2, &d1);   lmDet21 (&lx1, &lx2, &d2);
  lmMult (&k1, &d1, &p1);      lmMult (&k2, &d2, &p2);

  lmSub (&p2, &p1);

  if (! (result = lmSign (&p1)))
    if (! (result = x[0]))
      if (! (result = -y[0])) {
        printf("ERROR: incircleInf: 1 infinite point: result == 0 !\n");
        exit (1);
      }

  return result > 0;
}

/*---------------------------------------------------------------------------*/

int 
leftTurnInf (g, s0, s1, s2, detdbl)

     graphType *g;
     indexType s0, s1, s2;
     double *detdbl;

{
  static int site[3];
  int i, j, tmp, result, nswaps = 0;
  coordType x[3], y[3];

  if ((s0 >= 0) && (s1 >= 0) && (s2 >= 0))
    return leftTurn (g, s0, s1, s2, detdbl);

  site[0] = s0; site[1] = s1; site[2] = s2;

  for (i = 2; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (SITEN (g, site[j]) > SITEN(g, site[j+1])) {
	tmp = site[j]; site[j] = site[j+1]; site[j+1] = tmp;
	nswaps++;
      }
    }
  }

  if (site[0] >= 0) copyFinitePoints (g, 0, site[0], x, y);
  else copyInfinitePoints (g, 0, site[0], x, y);
  if (site[1] >= 0) copyFinitePoints (g, 1, site[1], x, y);
  else copyInfinitePoints (g, 1, site[1], x, y);
  if (site[2] >= 0) copyFinitePoints (g, 2, site[2], x, y);
  else copyInfinitePoints (g, 2, site[2], x, y);

/*  printf ("leftTurnInf: %d (%d %d), %d (%d %d), %d (%d %d) == ",
	  site[0], x[0], y[0], site[1], x[1], y[1], site[2], x[2], y[2]);
*/
  /* now the infinite points are on top of the list */

  if (site[2] < 0) 
    /* first case: 3 infinite points */
    result = leftTurn3Inf (x, y);
  else
    if (site[1] < 0)
      /* second case: 2 infinite points */
      result = leftTurn2Inf (x, y);
    else
      /* third case: 1 infinite point */
      result = leftTurn1Inf (x, y);

  if (nswaps & 1) {
    result = ! result;
    *detdbl = - (*detdbl);
  }

/*  printf("%d.\n", result);*/

  if (sosDebug) {
    (void) printf("incircle: %d %d %d (%d swaps) ", s0,s1,s2,nswaps);
    (void) printf(" answer: %d\n", result);
  }

  *detdbl = 0;
  return result;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/











