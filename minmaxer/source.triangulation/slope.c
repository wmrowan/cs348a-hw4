#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "menu.h"
#include "visual.h"
#include "graph.h"
#include "triation.h"

/*---------------------------------------------------------------------------*/

/*                     MINMAX SLOPE TRIANGULATION */

/*---------------------------------------------------------------------------*/

buildMinMaxSlopeTriangulation (mm, visual, nofFlips, nofAttempts, runTime)

     graphType *mm;
     visualType * visual;
     int * nofFlips, * nofAttempts;
     double * runTime;

{
  ANGLEMAKETIME = 0;
  NOFANGLECOMPARISONS = 0;

  printf ("MinMax Slope: "); (void) fflush(stdout);

  edgeInsertionN3 (mm, visual, heap (), sosSlopeGT, 
		   GT_CANNOT_DECIDE_RIGHT_TURN, sosSlopeRightTurn,
		   lmPrintSlope, nofFlips, nofAttempts, runTime);


  printf ("Time used for angle comparisons: %f\n", ANGLEMAKETIME);
  printf ("Number of Angle Comparisons: %d\n",   NOFANGLECOMPARISONS);
}

/*---------------------------------------------------------------------------*/

void
minmaxSlope (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  buildMinMaxSlopeTriangulation
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

