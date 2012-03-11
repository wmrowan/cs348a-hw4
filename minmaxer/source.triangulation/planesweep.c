/*
 * NAME: triations.c  contains code for various triangulations
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include "road.h"
#include "file_io.h"
#include "longmath.h"
#include "flips.h"
#include "queue.h"
#include "heap.h"
#include "menu.h"
#include "visual.h"
#include "graph.h"
#include "triation.h"
#include "triangulation.h"

/*---------------------------------------------------------------------------*/

/*                     INITIAL TRIANGULATION */

/*---------------------------------------------------------------------------*/
/*
 *   Compute an arbitrary triangulation of a set of points, based on
 * the "beneath-beyond" algorithm. See Edelsbrunner "Algorithms in
 * Combinitorial Geometry," pp. 143.
 */

/* this is a slightly modified version of pat's original */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int LTsite (siteTbl, s1, s2)
siteType siteTbl[];
int s1, s2;
{
  if (siteTbl[s1].coord[0] != siteTbl[s2].coord[0])
    return (siteTbl[s1].coord[0] < siteTbl[s2].coord[0]);

  return (siteTbl[s1].coord[1] < siteTbl[s2].coord[1]);
} 

/*---------------------------------------------------------------------------*/

void 
beneathBeyond (g, visual, runTime)

     graphType * g;
     visualType * visual;
     double * runTime;

{
  indexType n, qe1, qe2, qe3, startedge, qe;
  indexType newch_u, newch_l, oldch;
  double dontcare;

  (void) printf("Beneath Beyond:  ");  (void) fflush(stdout);

  *runTime = get_user_time ();

  quickSortSite(g->s, NS(g), LTsite, swapSite);

  qe1 = qeAddSiteSite(g, 0, 1);		/* build initial triangle */
  (*visual->drawEdge) (g, visual, qe1, 1);
INTERRUPT_ALGORITHM;

  qe2 = qeAddEdgeSite(g, qe1, 2);
  (*visual->drawEdge) (g, visual, qe2, 1);
INTERRUPT_ALGORITHM;

  qe3 = qeAddEdgeEdge(g, qe2, qe1);
  (*visual->drawEdge) (g, visual, qe3, 1);
INTERRUPT_ALGORITHM;

  /* startedge's Dest is site 2, and it follows ccw on triangle */
  if (leftTurn(g, 0, 1, 2, &dontcare))
    startedge = qe2;
  else
    startedge = SYM(qe3);

  newch_l = startedge;

  g->nChEdges = 3;

  for (n=3; n < NS(g); n++) {
    qe = qeAddEdgeSite(g, startedge, n);
    (*visual->drawEdge) (g, visual, qe, 1);
INTERRUPT_ALGORITHM;

    /* add edges above new edge qe (from site n-1 to site n) */
    newch_u = qe;
    oldch = ONEXT(g, qe);
    while (! leftTurn(g, n, ORG(g, oldch), DST(g, oldch), &dontcare)) {
      newch_u = qeAddEdgeEdge(g, oldch, SYM(qe));
      oldch = ONEXT(g, newch_u);
      (*visual->drawEdge) (g, visual, newch_u, 1);
      (g->nChEdges)--;
INTERRUPT_ALGORITHM;
    }

    /* add edges below new edge qe (from site n-1 to site n) */
    newch_l = qe;
    oldch = startedge;
    while (leftTurn(g, n, DST(g, oldch), ORG(g, oldch), &dontcare)) {
      oldch = SYM(OPREV(g, oldch));
      newch_l = qeAddEdgeEdge(g, oldch, SYM(newch_l));
      (*visual->drawEdge) (g, visual, newch_l, 1);
      (g->nChEdges)--;
INTERRUPT_ALGORITHM;
    }
    startedge = newch_l;
    g->nChEdges += 2;
  }


  /* save an arbitrary ccw convex edge */
  g->chEdge = newch_l;

  *runTime = get_user_time () - *runTime;
  (void) printf(" completed, cpu used %f.\n\n",
		*runTime);

INTERRUPT_LABEL: ;
}

/*---------------------------------------------------------------------------*/

void
buildInitialTriangulation (g, visual, runTime)

     graphType * g;
     visualType * visual;
     double * runTime;

{
  NE(g) = 0;
  beneathBeyond (g, visual, runTime);

  /* save number of edges and facets in closest point graph alone */
  g->nDtcEdges = NE(g);
  g->nDtcFacets = g->nDtcEdges - NS(g) + 1;	/* Euler nf = ne - ns + 1 */

  /* renumber the edges so the convex edges are numbered 0 .. (nChEdges-1) */
  /*qeRenumberEdges(g); */

  qeResetALLstati (g);
  qeSetCHstatus (g);
}

/*---------------------------------------------------------------------------*/

void
planeSweep (gExternal)

     char *gExternal;

{
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  buildInitialTriangulation (g, visual, &runTime);

  free (visual);
}
  
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

