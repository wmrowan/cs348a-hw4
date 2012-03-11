#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "menu.h"
#include "visual.h"
#include "graph.h"
#include "triation.h"

/*---------------------------------------------------------------------------*/

/*                     DELAUNAY TRIANGULATION */

/*---------------------------------------------------------------------------*/

bool
incirc (g, qe)

     graphType * g;
     indexType qe;

{
  double dummy;

  return (bool) incircle (g, DST(g, qe), DST(g, ONEXT(g, qe)), ORG(g, qe),
			  DST(g, ONEXT(g, SYM(qe))), &dummy);
}

/*---------------------------------------------------------------------------*/
     
buildDelaunayTriangulation (dt, visual, nofFlips, nofAttempts, runTime)

  graphType *dt;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  printf ("Delaunay: "); (void) fflush(stdout);

  lawsonFlip (dt, visual, queue (), incirc, (void *) 0,
	      nofFlips, nofAttempts, runTime);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
delaunay1 (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  buildDelaunayTriangulation 
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}
  
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

