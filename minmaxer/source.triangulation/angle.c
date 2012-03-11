#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "menu.h"
#include "visual.h"
#include "graph.h"
#include "triation.h"

/*---------------------------------------------------------------------------*/

/*                     MINMAX ANGLE TRIANGULATION */

/*---------------------------------------------------------------------------*/

// buildMinMaxAngleTriangulation2 (graphType*, visualType*, int*, int*, double*);

buildMinMaxAngleTriangulation (mm, visual, nofFlips, nofAttempts, runTime)

  graphType *mm;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  ANGLEMAKETIME = 0;
  NOFANGLECOMPARISONS = 0;

  printf ("MinMax Angle: "); (void) fflush(stdout);

  edgeInsertionN2LOGN (mm, visual, heap (), sosAngleGT, 
		       GT_CAN_DECIDE_RIGHT_TURN, sosAngleGT180,
		       lmPrintAngle, nofFlips, nofAttempts, runTime);


  printf ("Time used for angle comparisons: %f\n", ANGLEMAKETIME);
  printf ("Number of Angle Comparisons: %d\n",   NOFANGLECOMPARISONS);
}

/*---------------------------------------------------------------------------*/
     
buildMinMaxAngleTriangulation2 (mm, visual, nofFlips, nofAttempts, runTime)

  graphType *mm;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  ANGLEMAKETIME = 0;
  NOFANGLECOMPARISONS = 0;

  printf ("MinMax Angle: "); (void) fflush(stdout);

  edgeInsertionN3 (mm, visual, heap (), sosAngleGT, 
		   GT_CAN_DECIDE_RIGHT_TURN, sosAngleGT180,
		   lmPrintAngle, nofFlips, nofAttempts, runTime);


  printf ("Time used for angle comparisons: %f\n", ANGLEMAKETIME);
  printf ("Number of Angle Comparisons: %d\n",   NOFANGLECOMPARISONS);
}

/*---------------------------------------------------------------------------*/

void
minmaxAngle (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  buildMinMaxAngleTriangulation
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
