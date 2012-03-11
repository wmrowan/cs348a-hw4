/*
 * NAME: Flips.c
 *
 * DESCRIPTION:
 *	Contains structures for the (lawson type) edge flip,
 *      the (minmax angle type) edge insertion, and the 
 *      (minmax slope type) edge insertion algorithm.
 *
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include "road.h"
#include "menu.h"
#include "visual.h"
#include "quadedge.h"
#include "graph.h"
#include "flips.h"

/*---------------------------------------------------------------------------*/

static queuesType *
initializePool(g, pool, GTfunction)

     graphType *g;
     poolType  *pool;
     bool      (*GTfunction) ();
{
  int i;
  queuesType * tree;
  indexType qe;

  tree = (*pool->create) (NE(g), g, GTfunction, POOL_FIXED_SIZE);

  for (i = 0; i < NE(g); i++)
    if (! ISCHedge (g, i))
      (*pool->insert) (tree, i);

  return tree;
}

/*---------------------------------------------------------------------------*/

void
lawsonFlip (g, visual, pool, flipable, GTfunction,
	    nofFlips, nofAttempts, runTime)

     graphType  *g;
     int        *nofFlips, *nofAttempts;
     poolType   *pool;
     bool       (*flipable) (), (*GTfunction) ();
     visualType *visual;
     double * runTime;

{
  bool      foundEdge;
  indexType qe, i;
  indexType candidateEdge[4];
  queuesType *edges;
  indexType edge;

  printf ("Lawson Flip:  ");  (void) fflush (stdout);

  *runTime = get_user_time ();
  edges = initializePool (g, pool, GTfunction);

  *nofFlips = 0;
  *nofAttempts = 0;

  do {
    foundEdge = (*pool->top) (edges, &edge);

    if (foundEdge) {

      (*pool->delete) (edges, edge);


      qe = MAKEQE (edge);

      if ((*flipable) (g, qe)) {

	candidateEdge[0] = ONEXT(g, qe);
	candidateEdge[1] = OPREV(g, qe);
	candidateEdge[2] = ONEXT(g, SYM(qe));
	candidateEdge[3] = OPREV(g, SYM(qe));
	for (i = 0; i < 4; i++) {
	  edge = QETOE(candidateEdge[i]);
	  if (! ((ISCHedge (g, edge)) ||
		 ((*pool->doesContain) (edges, edge)))) {
	    (* pool->insert) (edges, edge);
	  }
        }

	(*visual->eraseEdge) (g, visual, qe);
INTERRUPT_ALGORITHM;	

	qeFlip (g, qe);

	(*visual->drawEdge) (g, visual, qe, 1);
INTERRUPT_ALGORITHM;	

	(*nofFlips)++;

      } else {
	(*nofAttempts)++;
      }
    }

  } while (foundEdge);

INTERRUPT_LABEL: ;
  (*pool->dispose) (edges);
if (! SHOULD_INTERRUPT_ALGORITHM) {
  *runTime = get_user_time () - *runTime;
  printf ("completed, cpu used ... %f,\n", *runTime);
  printf ("                       # flips ... %d, #attempts %d.\n\n",
	  *nofFlips, *nofAttempts);
}
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
