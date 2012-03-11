/*
 * NAME: triations.c  contains code for various triangulations
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include "road.h"
#include "file_io.h"
#include "graph.h"
#include "triangulation.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* external interfaces for triangulation algs */

/*---------------------------------------------------------------------------*/

void
saveTriangulation (gExternal, fileName)

     char *gExternal;
     char *fileName;

{
  graphType *g;

  g = (graphType *) gExternal;

  writeGraph(fileName, g, (int) 0, (int) 0, (double) 0.0);
}

/*---------------------------------------------------------------------------*/

void
copyCoordinatesToGraph (nofSites, x, y, z, eliminateDuplicates, gExternal)

     int nofSites;
     int * x, *y, *z;
     int eliminateDuplicates;
     char ** gExternal;

{
  graphType *g;

  *gExternal = (char *) newGraph ();

  g = (graphType *) *gExternal;

  gphAllocate(g, nofSites, nofSites * 3 - 6, 0, 0);
  if (eliminateDuplicates) gphUniqSites(g);
  
  while (NS(g) < nofSites) {
    gphLoadSite (g, x[NS(g)], y[NS(g)], (z != NULL ? z[NS(g)] : 0));
  }
}
  
/*---------------------------------------------------------------------------*/

triangleList *
makeTriangleList (nofTriangles)

     int nofTriangles;

{
  triangleList *tmp;

  if ((tmp = (triangleList *) malloc (sizeof (triangleList))) == NULL) {
    (void) printf ("ERROR: makeTriangleList: malloc failed");
    exit (1);
  }

  tmp->v = (triangleType *) calloc ((unsigned int) nofTriangles, 
				    sizeof (triangleType));
  if (tmp->v == NULL) {
    (void) printf ("ERROR: makeTriangleList: calloc failed");
    exit (1);
  }

  tmp->nofTriangles = 0;
  tmp->maxTriangles = nofTriangles;

  return tmp;
}

/*---------------------------------------------------------------------------*/

void
copyGraphToListOfTriangles (gExternal, tl)
     
     char *gExternal;
     triangleList ** tl;

{
  graphType *g;
  indexType edge, qe;

  g = (graphType *) gExternal;

  *tl = makeTriangleList (2 * NS (g) - 5);

  for (edge = 0; edge < NE(g); edge++) {
    qe = MAKEQE (edge);
    do {
      if (! (ISDELETEDQE (g, qe) || ISDELETEDQE (g, ONEXT (g, qe)) ||
	     ISDELETEDQE (g, OPREV (g, SYM (qe)))))
	if (DST (g, ONEXT (g, qe)) == DST (g, OPREV (g, SYM (qe))))
	  if ((qe < SYM (ONEXT (g, qe))) && (qe < OPREV (g, SYM (qe)))) {
	    if ((*tl)->nofTriangles >= (*tl)->maxTriangles) {
	      printf ("ERROR: copyGraphToListOfTriangles\n");
	      exit (1);
	    }
	    (*tl)->v[(*tl)->nofTriangles][0] = SITEN (g, ORG (g, qe));
	    (*tl)->v[(*tl)->nofTriangles][1] = SITEN (g, DST (g, qe));
	    (*tl)->v[(*tl)->nofTriangles][2] = 
	      SITEN (g, DST (g, ONEXT (g, qe)));
	    (*tl)->nofTriangles += 1;
	  }
      qe = SYM (qe);
    } while (qe != MAKEQE (edge));
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

