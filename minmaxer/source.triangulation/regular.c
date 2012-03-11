/*
 * NAME: regular.c
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include "road.h"
#include "menu.h"
#include "visual.h"
#include "quadedge.h"
#include "graph.h"
#include "hdag.h"

/* #include "regular.h"*/

extern int leftTurnInf ();

/*---------------------------------------------------------------------------*/

static int
returnFalse ()

{
  return 0;
}

/*---------------------------------------------------------------------------*/

static queuesType *
initializePool(g, pool, GTfunction)

     graphType *g;
     poolType  *pool;
     bool      (*GTfunction) ();
{
  return (*pool->create) (3*NS(g), g, GTfunction, POOL_ADJUSTABLE_SIZE);
}

/*---------------------------------------------------------------------------*/

insertFirst3vertices (g, visual)

     graphType *g;
     visualType *visual;


/*
  -2              -1
    *<------------*  
     \           /   
      \    0    /    
       \       /     
        \     / 
         \   /       
          \ /        
           * -3        
*/

/* the coordinates of these points are stored in g->infP */

{
  indexType qe1, qe2, qe3;

  qe1 = pqeAddSiteSite(g, -1, -2);        
  qe2 = pqeAddEdgeSite(g, qe1, -3);
  qe3 = pqeAddEdgeEdge(g, qe2, qe1);
  
  g->topTriangleEdge = qe2;
}

/*---------------------------------------------------------------------------*/

void
insertNextPoint (g, visual, p, pool, edges, regTest, spl)

     graphType *g;
     visualType *visual;
     indexType p;
     poolType   *pool;
     queuesType *edges;
     bool (*regTest) ();
     int *spl;

{
  indexType qe, qeSymNextSym, qeSymPrevSym, tmp;
  int count;

  qe = hdagInsertPoint (g, p, leftTurnInf, regTest, spl);

  if (qe != 0) {
    qeSymNextSym = pSYM (pONEXT (g, pSYM(qe), TIME_NOW));
    qeSymPrevSym = pSYM (pOPREV (g, pSYM(qe), TIME_NOW));

    (*visual->drawEdge) (g, visual, qe, 1);
INTERRUPT_ALGORITHM;
    (*visual->drawEdge) (g, visual, qeSymNextSym, 1);
INTERRUPT_ALGORITHM;
    (*visual->drawEdge) (g, visual, qeSymPrevSym, 1);
INTERRUPT_ALGORITHM;

/*    printf ("insertNextPoint: %d %d %d\n", pORG (g, qe), pORG (g, qeSymNextSym), 
	    pORG (g, qeSymPrevSym));
*/    
    count = 0;
    if (pORG (g, qe) >= 0)
      count ++;
    if (pORG (g, qeSymNextSym) >= 0)
      count ++;
    if (pORG (g, qeSymPrevSym) >= 0)
      count ++;

    if (count == 1)
      if (pORG (g, qeSymNextSym) >= 0) {
	tmp = qe; qe = qeSymNextSym; qeSymNextSym = qeSymPrevSym;
	qeSymPrevSym = tmp;
      } else
	if (pORG (g, qeSymPrevSym) >= 0) {
	  tmp = qe; qe = qeSymPrevSym; qeSymPrevSym = qeSymNextSym; 
	  qeSymNextSym = tmp;
	}

    (* pool->insert) (edges, pQETOE (pONEXT(g, qe, TIME_NOW)));
    (* pool->insert) (edges, pQETOE (pONEXT(g, qeSymNextSym, TIME_NOW)));
    (* pool->insert) (edges, pQETOE (pONEXT(g, qeSymPrevSym, TIME_NOW)));
  } else {
/*    printf ("insertNextPoint: %d not inserted\n", p);*/
  }
INTERRUPT_LABEL: ;
}

/*---------------------------------------------------------------------------*/

static void
do22flip (g, visual, pool, edges, qe, vertex)

     graphType *g;
     visualType *visual;
     poolType   *pool;
     queuesType *edges;
     indexType qe;
     indexType vertex;

{
  indexType flipQe, edge;

  (*visual->eraseEdge) (g, visual, qe);
INTERRUPT_ALGORITHM;  
  flipQe = pqeFlip (g, qe);
  (*visual->drawEdge) (g, visual, flipQe, 1);
INTERRUPT_ALGORITHM;

  if (((pDST (g, flipQe) != vertex) && (pORG (g, flipQe) != vertex)))
    printf("ERROR\n");

  /*    printf ("b %d %d %d %d %d\n", flipQe, pONEXT(g, pSYM(flipQe), TIME_NOW),        pOPREV(g, pSYM(flipQe), TIME_NOW), pONEXT(g, flipQe, TIME_NOW),
        pOPREV(g, flipQe, TIME_NOW));*/
  
  if (pORG (g, flipQe) == vertex) 
    flipQe = pSYM (flipQe);

  edge = pQETOE(pONEXT(g, flipQe, TIME_NOW));
  
  if (! (*pool->doesContain) (edges, edge)) {
    (* pool->insert) (edges, edge);
  }

  edge = pQETOE(pOPREV(g, flipQe, TIME_NOW));
  
  if (! ((pISCHedge (g, edge, TIME_NOW)) ||
	 ((*pool->doesContain) (edges, edge)))) {
    (* pool->insert) (edges, edge);
  }
INTERRUPT_LABEL: ;
}

/*---------------------------------------------------------------------------*/

static int
do31flip (g, visual, pool, edges, flipQe, vertex)

     graphType *g;
     visualType *visual;
     poolType   *pool;
     indexType flipQe;
     indexType vertex;
     queuesType *edges;

{
  indexType edge;
  int tmp, success;

/*  printf("ERROR: halli hallo\n");*/

  if (pDST (g, pOPREV(g, pSYM (flipQe), TIME_NOW)) ==
    pDST (g, pOPREV(g, pSYM (pONEXT(g, pSYM (flipQe), TIME_NOW)), TIME_NOW))) {

    (*visual->eraseEdge) (g, visual, flipQe);
INTERRUPT_ALGORITHM;
    (*visual->eraseEdge) (g, visual, pOPREV(g, pSYM(flipQe), TIME_NOW));
INTERRUPT_ALGORITHM;
    (*visual->eraseEdge) (g, visual, pONEXT(g, pSYM(flipQe), TIME_NOW));
INTERRUPT_ALGORITHM;

    tmp = (pDST (g, pONEXT (g, flipQe, TIME_NOW)) == vertex);

    if (tmp)
      edge = pQETOE (pOPREV (g, flipQe, TIME_NOW));
    else
      edge = pQETOE (pONEXT (g, flipQe, TIME_NOW));

    if (! (*pool->doesContain) (edges, edge)) {
      (*pool->insert) (edges, edge);
    }

    if (tmp)
      edge = pQETOE (pONEXT (g, pSYM(flipQe), TIME_NOW));
    else
      edge = pQETOE (pOPREV (g, pSYM(flipQe), TIME_NOW));

    if ((*pool->doesContain) (edges, edge)) {
      (*pool->delete) (edges, edge);
    }

    pqeDeleteTriangleStar (g, pOPREV(g, flipQe, TIME_NOW));

    success = 1;
  } else { /* can't flip */
    if (pDST (g, pONEXT (g, flipQe, TIME_NOW)) == vertex)
      edge = pQETOE (pONEXT (g, pONEXT (g, pSYM(flipQe), TIME_NOW), TIME_NOW));
    else
      edge = pQETOE (pOPREV (g, pOPREV (g, pSYM(flipQe), TIME_NOW), TIME_NOW));

    if (! (*pool->doesContain) (edges, edge)) {
      (*pool->insert) (edges, edge);
    }
    success = 0;
  }

INTERRUPT_LABEL: ;
  return success;
}

/*---------------------------------------------------------------------------*/

static int
doFlipEdge (g, visual, pool, edges, qe, vertex)

     graphType *g;
     visualType *visual;
     poolType   *pool;
     indexType qe;
     indexType vertex;
     queuesType *edges;

{
  double dummy;
  int success;

  if (leftTurnInf (g, pDST (g, pONEXT(g, qe, TIME_NOW)), pORG (g, qe),
		   pDST (g, pOPREV(g, qe, TIME_NOW)), &dummy))
    if (leftTurnInf (g, pDST (g, pOPREV(g, qe, TIME_NOW)), pDST (g, qe),
		     pDST (g, pONEXT(g, qe, TIME_NOW)), &dummy)) {

      do22flip (g, visual, pool, edges, qe, vertex);
      success = 1;

    } else {
      success = do31flip (g, visual, pool, edges, qe, vertex);

  } else {

    success = do31flip (g, visual, pool, edges, pSYM (qe), vertex);
  }
  return success;
}

/*---------------------------------------------------------------------------*/

static bool
inCircInf (g, qe)

     graphType * g;
     indexType qe;

{
  double dummy;

  if ((pDST(g, qe) < 0) && (pORG(g, qe) < 0))
    return 0;

  return (bool) inCircleInf (g, pDST(g, qe), 
			     pDST(g, pONEXT(g, qe, TIME_NOW)), 
			     pORG(g, qe), 
			     pDST(g, pONEXT(g, pSYM(qe), TIME_NOW)), 
			     1,
			     &dummy);
}

/*---------------------------------------------------------------------------*/

static bool
regularityTestInf (g, qe)

     graphType * g;
     indexType qe;

{
  double dummy;

  if ((pDST(g, qe) < 0) && (pORG(g, qe) < 0))
    return 0;

  return (bool) inCircleInf (g, pDST(g, qe), 
			     pDST(g, pONEXT(g, qe, TIME_NOW)), 
			     pORG(g, qe), 
			     pDST(g, pONEXT(g, pSYM(qe), TIME_NOW)), 
			     0,
			     &dummy);
}

/*---------------------------------------------------------------------------*/

static bool
inCircVerticesInf (g, s0, s1, s2, s3)

     graphType * g;
     indexType s0, s1, s2, s3;

{
  double dummy;

  if ((s0 < 0) && (s2 < 0))
    return 0;

  return (bool) inCircleInf (g, s0, s1, s2, s3, 1, &dummy);
}

/*---------------------------------------------------------------------------*/

static bool
regularityTestVerticesInf (g, s0, s1, s2, s3)

     graphType * g;
     indexType s0, s1, s2, s3;

{
  double dummy;

  if ((s0 < 0) && (s2 < 0))
    return 0;

  return (bool) inCircleInf (g, s0, s1, s2, s3, 0, &dummy);
}

/*---------------------------------------------------------------------------*/

static indexType *
initialEdgeP (g)

     graphType *g;

{
  indexType * tmp;
  indexType vertex;

  tmp = (indexType *) calloc((unsigned int) NOS(g), 
			     sizeof(indexType));
  if (tmp == NULL) {
    printf("initialEdgeP: fail on table allocation.\n");
    exit (1);
  }

  for (vertex = 0; vertex < NOS(g); vertex++) 
    tmp [vertex] = -1;

  return tmp;
}

/*---------------------------------------------------------------------------*/

void
verifyRegular (g, doDelaunay)

     graphType *g;
     int doDelaunay;

{
  bool (*testRegular)();
  indexType edge, qe, vertex, qeSym;
  int failCount;
  char fname[50];
  FILE *fp, *fopen();
  indexType *edgeP; /* pointers from vertices to one edge incident to */
  /* the vertex */

  printf ("testing %s: ", (doDelaunay ? "delaunayness" : "regularity"));
  (void) fflush(stdout);

  if ((fp = fopen ((doDelaunay ? "delaunay.verify" : "regular.verify"), "w")) == NULL) {
    (void) printf("verifyRegular:  can't open \"%s\" to write.\n", fname);
    return;
  }

  if (doDelaunay == 1)
    testRegular = inCircVerticesInf;
  else
    testRegular = regularityTestVerticesInf;

  failCount = 0;

  for (edge = 0; edge < NE (g); edge++) {
    qe = MAKEQE (edge);
    qeSym = SYM (qe);
    
    if (! ISCHedge(g, edge)) {
      if (ONEXT (g, qe) != SYM (OPREV (g, SYM (OPREV (g, SYM (qe)))))) {
	fprintf (fp, "not triangle %d\n", qe);
	failCount++;
      }
      if (ONEXT (g, qeSym) != SYM (OPREV (g, SYM (OPREV (g, SYM (qeSym)))))) {
	fprintf (fp, "not triangle %d\n", qeSym);
	failCount++;
      }
    } else 
      if ((ONEXT (g, qe) != SYM (OPREV (g, SYM (OPREV (g, SYM (qe)))))) &&
	  (ONEXT (g, qeSym) != SYM (OPREV (g, SYM (OPREV (g, SYM (qeSym))))))){
	fprintf (fp, "not triangle ch edge %d\n", qe);
	failCount++;
      }
  }

  if (! failCount)
    printf ("\nTriangulation test: Graph is a triangulation.");

  for (edge = 0; edge < NE (g); edge++) {
    qe = MAKEQE (edge);
    qeSym = SYM (qe);
    vertex = DST(g, OPREV(g, qe));
    if ((vertex != ORG(g, qe)) && (vertex != DST(g, qe)) &&
	(vertex != DST(g, ONEXT(g, qe))))
      if (ONEXT (g, qe) == SYM (OPREV (g, SYM (OPREV (g, SYM (qe))))))
	if (! (*testRegular) (g, ORG(g, qe), DST(g, ONEXT(g, qe)),
			      DST(g, qe), vertex)) {
	  fprintf (fp, "not %s edge (%d, %d), %d %d\n",
		   (doDelaunay ? "delaunay" : "regular"), 
		   ORG(g, qe), DST(g, qe), DST(g, ONEXT(g, qe)),
		   vertex);
	  failCount++;
	}
    
    vertex = DST(g, ONEXT(g, qe));
    if ((vertex != ORG(g, qe)) && (vertex != DST(g, qe)) &&
	(vertex != DST(g, OPREV(g, qe))))
      if (ONEXT (g, qeSym) == SYM (OPREV (g, SYM (OPREV (g, SYM (qeSym))))))
	if (! (*testRegular) (g, ORG(g, qe), vertex, DST(g, qe),
			      DST(g, OPREV(g, qe)))) {
	  fprintf (fp, "not %s edge (%d, %d), %d %d\n",
		   (doDelaunay ? "delaunay" : "regular"), 
		   ORG(g, qe), DST(g, qe), DST(g, OPREV(g, qe)),
		   vertex);
	  failCount++;
	}
  }

  if (! failCount)
    printf ("\nAll edges are locally %s.", (doDelaunay ? "delaunay" : "regular"));

  edgeP = initialEdgeP (g);

  for (edge = 0; edge < NE (g); edge++) {
    qe = MAKEQE (edge);
    qeSym = SYM (qe);
    
    edgeP [ORG(g, qe)] = qe;
    edgeP [ORG(g, qeSym)] = qeSym;
  }

  for (vertex = 0; vertex < NOS (g); vertex++) 
    if (edgeP [vertex] == -1) 
      for (edge = 0; edge < NE (g); edge++) {
	qe = MAKEQE (edge);
	qeSym = SYM (qe);
	if ((vertex != ORG(g, qe)) && (vertex != DST(g, qe)) &&
	    (vertex != DST(g, ONEXT(g, qe))))
	  if (ONEXT (g, qe) == SYM (OPREV (g, SYM (OPREV (g, SYM (qe))))))
	    if (! (*testRegular) (g, ORG(g, qe), DST(g, ONEXT(g, qe)),
				  DST(g, qe), vertex)) {
	      fprintf (fp, "not %s edge (%d, %d), %d %d\n",
		       (doDelaunay ? "delaunay" : "regular"), 
		       ORG(g, qe), DST(g, qe), DST(g, ONEXT(g, qe)),
		       vertex);
	      failCount++;
	    }
	
	if ((vertex != ORG(g, qe)) && (vertex != DST(g, qe)) &&
	    (vertex != DST(g, OPREV(g, qe))))
	  if (ONEXT (g, qeSym) == SYM (OPREV (g, SYM (OPREV (g, SYM (qeSym))))))
	    if (! (*testRegular) (g, ORG(g, qe), vertex, DST(g, qe),
				  DST(g, OPREV(g, qe)))) {
	      fprintf (fp, "not %s edge (%d, %d), %d %d\n",
		       (doDelaunay ? "delaunay" : "regular"), 
		       ORG(g, qe), DST(g, qe), DST(g, OPREV(g, qe)),
		       vertex);
	      failCount++;
	    }
      }

  (void) free ((char *) edgeP);

  if (! failCount) {
    fprintf (fp, "\ntriangulation is globally %s\n", 
	    (doDelaunay ? "delaunay" : "regular"));
    printf ("\ntriangulation is globally %s\n", 
	    (doDelaunay ? "delaunay" : "regular"));
  } else {
    fprintf (fp, "\nERROR: triangulation is not globally %s\n",
	    (doDelaunay ? "delaunay" : "regular"));
    printf ("\n\nERROR: triangulation is not globally %s\n",
	    (doDelaunay ? "delaunay" : "regular"));
    printf ("       more info in file %s.\n", 
	    (doDelaunay ? "delaunay.verify" : "regular.verify"));
  }

  fclose (fp);
}

/*---------------------------------------------------------------------------*/

void
incrementalFlip (gOrig, visual, pool, doDelaunay, GTfunction,
		 nofFlips, nofAttempts, runTime)

     graphType *gOrig;
     int        *nofFlips, *nofAttempts;
     poolType   *pool;
     int        doDelaunay;
     bool       (*GTfunction) ();
     visualType *visual;
     double * runTime;

{
  bool      foundEdge;
  indexType qe;
  queuesType *edges;
  indexType edge;
  indexType n;
  graphType theG, *g;
  int spl;
  bool (*testRegular)();
  bool (*testRegularVertices)();

  printf ("Incremental Flip: ");  (void) fflush (stdout);
  *runTime = get_user_time ();

  if (doDelaunay == 1) {
    testRegular = inCircInf;
    testRegularVertices = inCircVerticesInf;
  } else {
    testRegular = regularityTestInf;
    testRegularVertices = regularityTestVerticesInf;
  }

  g = &theG;

  randomizeOrder (gOrig->s, NS(gOrig), swapSite);

  NS (g) = NS (gOrig); g->s = gOrig->s;
  g->xMax = gOrig->xMax; g->xMin = gOrig->xMin; 
  g->yMax = gOrig->yMax; g->yMin = gOrig->yMin; 
  g->zMax = gOrig->zMax; g->zMin = gOrig->zMin; 
  g->infP[1][0] = gOrig->infP[1][0]; g->infP[1][1] = gOrig->infP[1][1];
  g->infP[2][0] = gOrig->infP[2][0]; g->infP[2][1] = gOrig->infP[2][1];
  g->infP[3][0] = gOrig->infP[3][0]; g->infP[3][1] = gOrig->infP[3][1];
  
  pqeNew (g, (indexType) 3*NS(g));
  resetHdag (g);

  edges = initializePool (g, pool, GTfunction);
  insertFirst3vertices (g, visual);

  *nofFlips = 0;
  *nofAttempts = 0;
  spl = 0;

  for (n = 0; n < NS (g); n++) {

/*    printf("%d %d %d\n", n, SITEX(g, n), SITEY(g, n));*/

    insertNextPoint (g, visual, n, pool, edges, testRegularVertices, &spl);
INTERRUPT_ALGORITHM;
    foundEdge = (*pool->top) (edges, &edge);

    while (foundEdge) {
      
      (*pool->delete) (edges, edge);
      
      qe = pMAKEQE (edge);
      
/*      printf("foundEdge: %d %d %d %d %d", qe, pORG (g, qe), pDST (g, qe),
	     pDST(g, pONEXT(g, qe, TIME_NOW)),
	     pDST(g, pOPREV(g, qe, TIME_NOW)));
*/

      
      if ((*testRegular) (g, qe)) {
	
/*	printf ("flipped\n");*/
	
	if (doFlipEdge (g, visual, pool, edges, qe, n))
	  (*nofFlips)++;
	else
	  (*nofAttempts)++;
INTERRUPT_ALGORITHM;	
      } else {

	(*nofAttempts)++;
/*	printf ("not flipped\n");*/

      }
      
      foundEdge = (*pool->top) (edges, &edge);
    }
  }

INTERRUPT_LABEL: ;
  (*pool->dispose) (edges);
  if (gOrig->e != NULL)
    free (gOrig->e);
  copyHdagToQe (g, -1, &(NE(gOrig)), &(gOrig->e), &(gOrig->nChEdges));
  printf("\n");
/*  printPqeStats (g);*/
  disposePqe (g);
if (! SHOULD_INTERRUPT_ALGORITHM) {
  *runTime = get_user_time () - *runTime;
  printf ("completed, cpu used ... %f,\n", *runTime);
  printf ("                       # flips ... %d, #attempts %d.\n",
	  *nofFlips, *nofAttempts);
/*  printf ("                       Hirachy: %d edges.\n", NE (g));*/
  printf ("                       Avg Search Path Length: %d.\n\n", 
	  spl / NS (g));
}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
     
newDelaunayTriangulation (dt, visual, nofFlips, nofAttempts, runTime)

  graphType *dt;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  printf ("Delaunay: "); (void) fflush(stdout);

  incrementalFlip (dt, visual, queue (), 1, (void *) 0,
		   nofFlips, nofAttempts, runTime);
}

/*---------------------------------------------------------------------------*/
     
regularTriangulation (dt, visual, nofFlips, nofAttempts, runTime)

  graphType *dt;
  visualType * visual;
  int * nofFlips, * nofAttempts;
  double * runTime;

{
  printf ("Regular: "); (void) fflush(stdout);

  incrementalFlip (dt, visual, queue (), 0, (void *) 0,
		   nofFlips, nofAttempts, runTime);
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

void
delaunay2 (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  newDelaunayTriangulation
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}

/*---------------------------------------------------------------------------*/

void
regular (gExternal)

     char *gExternal;

{
  int nofFlips, nofAttempts;
  double runTime;
  visualType *visual;
  graphType *g;
  
  g = (graphType *) gExternal;

  visual = dummyVisual ();
  regularTriangulation
    (g, visual, &nofFlips, &nofAttempts, &runTime);

  free (visual);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
