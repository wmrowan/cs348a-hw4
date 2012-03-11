#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "menu.h"
#include "visual.h"
#include "graph.h"
#include "triation.h"

/*---------------------------------------------------------------------------*/

/*                     MINMAX HEIGHT TRIANGULATION */

/*---------------------------------------------------------------------------*/
     
buildMaxMinHeightTriangulation (mm, visual, nofFlips, nofAttempts, runTime)

  graphType *mm;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  ANGLEMAKETIME = 0;
  NOFANGLECOMPARISONS = 0;

  printf ("MaxMin Height: "); (void) fflush(stdout);

  edgeInsertionN2LOGN (mm, visual, heap (), sosHeightLT, 
		       GT_CAN_DECIDE_RIGHT_TURN, sosHeightLT0, 
		       lmPrintHeight, nofFlips, nofAttempts, runTime);

  printf ("Time used for height comparisons: %f\n", ANGLEMAKETIME);
  printf ("Number of Height Comparisons: %d\n",   NOFANGLECOMPARISONS);
}

/*---------------------------------------------------------------------------*/
     
buildMaxMinHeightTriangulation2 (mm, visual, nofFlips, nofAttempts, runTime)

  graphType *mm;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  ANGLEMAKETIME = 0;
  NOFANGLECOMPARISONS = 0;

  printf ("MaxMin Height: "); (void) fflush(stdout);

  edgeInsertionN3 (mm, visual, heap (), sosHeightLT,
		   GT_CAN_DECIDE_RIGHT_TURN, sosHeightLT0, 
		   lmPrintHeight, nofFlips, nofAttempts, runTime);

  printf ("Time used for height comparisons: %f\n", ANGLEMAKETIME);
  printf ("Number of Height Comparisons: %d\n",   NOFANGLECOMPARISONS);
}

/*---------------------------------------------------------------------------*/

void
maxminHeight (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  buildMaxMinHeightTriangulation
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
