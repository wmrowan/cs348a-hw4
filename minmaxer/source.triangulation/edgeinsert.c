/*
  edgeinsert.c
*/

#include <stdio.h>
#include "road.h"
#include "quadedge.h"
#include "graph.h"
#include "stack.h"
#include "menu.h"
#include "visual.h"
#include "triation.h"
#include "bitvector.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define N3 0
#define N2LOGN 1

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* global variables */ 

static graphType *g, *gPrime;
static visualType * visual;
static poolType *pool;
static queuesType * edges;
static indexType Q, S, globS;
static indexType prevToOrigPR, symNextSymToPR;
static criterionType actMax;
static stackType * edgeStack;
static wedgeType wedge;
static int flips, attempts;
static bvType * qsIsNotVerified;
static int qsIsNotVerifiedMaxIndex;

static bool (*applicationGT) ();
static bool (*applicationRightTurn) ();
static void (*applicationPrintMeasure) ();

static bool (*localGTfun) ();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
debugPrintQE (g, name, qe, intend)

     graphType * g;
     char * name;
     indexType qe;
     int intend;

{
/*  int i;


  for (i = 1; i <= intend; i++)
    printf ("   ");

  printf ("%s: %d, %d, %d\n", name, ORG (g, qe), DST (g, qe), qe);
  fflush (stdout);
*/
}

/*---------------------------------------------------------------------------*/

void 
debugPrintQE2 (g, name, qe, intend)

     graphType * g;
     char * name;
     indexType qe;
     int intend;

{
/*  int i;
  void printEdgeMeasure ();

  qe <<= 1;

  for (i = 1; i <= intend; i++)
    printf ("   ");

  printf ("%s: %d, %d, %d\n", name, ORG (g, qe), DST (g, qe), qe);
  fflush (stdout);
*/
/*  printEdgeMeasure (g, qe);*/
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static bool
localGT (g, triple1, triple2)

     graphType * g;
     criterionType * triple1, * triple2;

{
  static bool tmp;

  ANGLEMAKETIME =   ANGLEMAKETIME - get_user_time();

  tmp = (*applicationGT) (
        triple1->center, SITEX(g, triple1->center),  SITEY(g,triple1->center),
			  SITEZ(g,triple1->center),
        triple1->left, SITEX(g, triple1->left),  SITEY(g,triple1->left),
			  SITEZ(g,triple1->left),
        triple1->right, SITEX(g, triple1->right),  SITEY(g,triple1->right),
			  SITEZ(g,triple1->right),
        triple2->center, SITEX(g, triple2->center),  SITEY(g,triple2->center),
			  SITEZ(g,triple2->center),
        triple2->left, SITEX(g, triple2->left),  SITEY(g,triple2->left),
			  SITEZ(g,triple2->left),
        triple2->right, SITEX(g, triple2->right),  SITEY(g,triple2->right),
			  SITEZ(g,triple2->right));

/*  printf("localGT %d\n", tmp);*/

  ANGLEMAKETIME =   ANGLEMAKETIME + get_user_time();
  NOFANGLECOMPARISONS++;

  return (bool) tmp;

}

/*---------------------------------------------------------------------------*/

static bool
localGTplusRightTurn (g, triple1, triple2)

     graphType * g;
     criterionType * triple1, * triple2;

{
  static bool tmp;

  ANGLEMAKETIME =   ANGLEMAKETIME - get_user_time();

  tmp = (
    (*applicationRightTurn) (
        triple1->center, SITEX(g, triple1->center),  SITEY(g,triple1->center),
				SITEZ(g,triple1->center),
        triple1->left, SITEX(g, triple1->left),  SITEY(g,triple1->left),
				SITEZ(g,triple1->left),
        triple1->right, SITEX(g, triple1->right),  SITEY(g,triple1->right),
				SITEZ(g,triple1->right))
    ||

    (*applicationGT) (
        triple1->center, SITEX(g, triple1->center), SITEY(g,triple1->center),
		      SITEZ(g,triple1->center),
        triple1->left, SITEX(g, triple1->left),  SITEY(g,triple1->left),
		      SITEZ(g,triple1->left),
        triple1->right, SITEX(g, triple1->right), SITEY(g,triple1->right),
		      SITEZ(g,triple1->right),
        triple2->center, SITEX(g, triple2->center), SITEY(g,triple2->center),
		      SITEZ(g,triple2->center),
        triple2->left, SITEX(g, triple2->left), SITEY(g,triple2->left),
		      SITEZ(g,triple2->left),
        triple2->right, SITEX(g, triple2->right), SITEY(g,triple2->right),
		      SITEZ(g,triple2->right))
  );

/*  printf("localGT %d %d\n", 
(
    (*applicationRightTurn) (
        triple1->center, SITEX(g, triple1->center),  SITEY(g,triple1->center),
                                SITEZ(g,triple1->center),
        triple1->left, SITEX(g, triple1->left),  SITEY(g,triple1->left),
                                SITEZ(g,triple1->left),
        triple1->right, SITEX(g, triple1->right),  SITEY(g,triple1->right),
                                SITEZ(g,triple1->right))), tmp);
*/
  ANGLEMAKETIME =   ANGLEMAKETIME + get_user_time();
  NOFANGLECOMPARISONS++;

  return (bool) tmp;

}

/*---------------------------------------------------------------------------*/

static bool
localGTedges (g, edge1, edge2)

     graphType * g;
     indexType edge1, edge2;

{
  static bool tmp;

  ANGLEMAKETIME =   ANGLEMAKETIME - get_user_time();

  tmp = (*applicationGT) (
        ORG (g, edge1), SITEX(g,  ORG (g, edge1)), SITEY(g, ORG (g, edge1)),
			  SITEZ(g, ORG (g, edge1)),
        DST (g, ONEXT (g, edge1)), SITEX(g, DST (g, ONEXT (g, edge1))),
			  SITEY(g, DST (g, ONEXT (g, edge1))), 
			  SITEZ(g, DST (g, ONEXT (g, edge1))),
        DST (g, edge1), SITEX(g, DST (g, edge1)), SITEY(g,DST (g, edge1)),
			  SITEZ(g,DST (g, edge1)),
        ORG (g, edge2), SITEX(g, ORG (g, edge2)), SITEY(g,ORG (g, edge2)),
			  SITEZ(g,ORG (g, edge2)),
        DST (g, ONEXT (g, edge2)), SITEX(g, DST (g, ONEXT (g, edge2))), 
			  SITEY(g,DST (g, ONEXT (g, edge2))),
			  SITEZ(g,DST (g, ONEXT (g, edge2))),
        DST (g, edge2), SITEX(g, DST (g, edge2)), SITEY(g,DST (g, edge2)),
			  SITEZ(g,DST (g, edge2)));

  ANGLEMAKETIME =   ANGLEMAKETIME + get_user_time();
  NOFANGLECOMPARISONS++;

  return (bool) tmp;
}

/*---------------------------------------------------------------------------*/

static bool
isLeft ()

{
  static bool  isNotInsideLeft;

  ANGLEMAKETIME =   ANGLEMAKETIME - get_user_time();
  NOFANGLECOMPARISONS++;

  isNotInsideLeft  = 
    (*applicationRightTurn) (S, SITEX(g, S),  SITEY(g, S), SITEZ(g, S),
			     globS, SITEX(g, globS),  SITEY(g, globS),
			     SITEZ(g, globS),
			     wedge.center, SITEX(g, wedge.center),  
			     SITEY(g,wedge.center), SITEZ(g,wedge.center));

  ANGLEMAKETIME =   ANGLEMAKETIME + get_user_time();

  return isNotInsideLeft;
}

/*---------------------------------------------------------------------------*/

static void
testInsideWedge (g, candidate, wedge, isInsideRight, isInsideLeft)

     graphType *g;
     indexType candidate;
     wedgeType *wedge;
     bool      *isInsideRight, *isInsideLeft;

{
  ANGLEMAKETIME =   ANGLEMAKETIME - get_user_time();

  *isInsideRight = 
    ! (*applicationRightTurn) (
        wedge->right, SITEX(g, wedge->right),  SITEY(g,wedge->right),
			       SITEZ(g,wedge->right),
	wedge->center, SITEX(g, wedge->center),  SITEY(g,wedge->center),
			       SITEZ(g,wedge->center),
        candidate, SITEX(g, candidate),  SITEY(g, candidate), 
			       SITEY(g, candidate));

  NOFANGLECOMPARISONS++;

  if (*isInsideRight) {
    *isInsideLeft  = 
      ! (*applicationRightTurn) (
        wedge->left, SITEX(g, wedge->left),  SITEY(g,wedge->left),
				 SITEZ(g,wedge->left),
        candidate, SITEX(g, candidate),  SITEY(g, candidate),
				 SITEZ(g, candidate),
        wedge->center, SITEX(g, wedge->center),  SITEY(g,wedge->center),
				 SITEZ(g,wedge->center));

    NOFANGLECOMPARISONS++;
  } else
    *isInsideLeft = 1;

  ANGLEMAKETIME =   ANGLEMAKETIME + get_user_time();
}

/*---------------------------------------------------------------------------*/

static void
printEdgeMeasure (g, edge1)

     graphType * g;
     indexType edge1;

{
  (*applicationPrintMeasure) (stdout, 
	ORG (g, edge1), SITEX(g,  ORG (g, edge1)), SITEY(g, ORG (g, edge1)),
			      SITEZ(g, ORG (g, edge1)),
        DST (g, ONEXT (g, edge1)), SITEX(g, DST (g, ONEXT (g, edge1))),
			      SITEY(g, DST (g, ONEXT (g, edge1))),
			      SITEZ(g, DST (g, ONEXT (g, edge1))),
        DST (g, edge1), SITEX(g, DST (g, edge1)), SITEY(g,DST (g, edge1)),
			      SITEZ(g,DST (g, edge1)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void
markFinalEdges (g)

     graphType * g;

{ 
  int i;

  for (i = 0; i < NE(g); i++)
    if ((ISCHedge (g, i)) || (ISCONSTRedge (g, i))) {
      SETFINALedge (g, i);
      debugPrintQE (g, "final", MAKEQE (i), 0);
    } else {
      UNSETFINALedge (g, i);
      debugPrintQE (g, "not final", MAKEQE (i), 0);
    }
}

/*---------------------------------------------------------------------------*/

static void
unmarkFinalEdges (g)

     graphType * g;

{ 
  qeResetFINALstatus (g);
}

/*---------------------------------------------------------------------------*/

static void
mkCopyOfG (g, gPrime)
  
     graphType *g;
     graphType **gPrime;

{  
  int i;

  *gPrime = newGraph ();
  **gPrime = *g;
  NE(*gPrime) = 0;
  gphAllocate (*gPrime, 0, NE (g), 0, 0);

  for (i = 0; i < NE(g); i++) {
    SETDELETEDedge (*gPrime, i);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define poolCreate(graph, fun) (*pool->create) (2 * NE(graph), graph, fun, POOL_FIXED_SIZE)
#define poolDispose(qePool) (*pool->dispose)
#define poolInsert(qePool, qe) (*pool->insert) (qePool, (qe) >> 1)
#define poolDelete(qePool, qe) (*pool->delete) (qePool, (qe) >> 1)
#define poolDoesContain(qePool, qe) (*pool->doesContain) (qePool, (qe) >> 1)

/*---------------------------------------------------------------------------*/

bool
poolTop (qePool, pqe)

     queuesType * qePool;
     indexType * pqe;

{
  bool found;

  found = (*pool->top) (qePool, pqe);
  *pqe <<= 1;

  return found;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static bool
GTfunctionForQEpool (g, qe1, qe2)

     graphType * g;
     indexType qe1, qe2;

{
  return localGTedges (g, qe1 << 1, qe2 << 1);
}

/*---------------------------------------------------------------------------*/

void
maxMeasureQE (g, qe, winner, winnerExists)

     graphType *g;
     indexType qe;
     indexType *winner;
     int       *winnerExists;

{
  indexType other;

  other =  OPREV (g, SYM( qe ));

  *winnerExists = 1;

  if (! ISFINALQE (g, OPREV (g, SYM( qe ))))
    if (! ISFINALQE (g, OPREV (g, SYM( other ))))
      *winner = (localGTedges (g, qe, other) ? qe : other);
    else
      *winner = qe;
  else 
    *winner = other;

  other = SYM (ONEXT (g, qe));

  if (! ISFINALQE (g, OPREV (g, SYM( *winner )))) {
    if (! ISFINALQE (g, OPREV (g, SYM( other )))) 
      if (localGTedges (g, other, *winner))
	*winner = other;
  } else 
    if (! ISFINALQE (g, OPREV (g, SYM( other ))))
      *winner = other;
    else
      *winnerExists = 0;
}

  
/*---------------------------------------------------------------------------*/

static queuesType *
initializePool(g, pool)

     graphType *g;
     poolType  *pool;

{
  int i;
  queuesType * tree;
  indexType qe;
  int exists;

  tree = poolCreate (g, GTfunctionForQEpool);
  
  for (i = 0; i < NE(g); i++) {
    maxMeasureQE (g, MAKEQE (i), &qe, &exists);
    if (exists && (! poolDoesContain (tree, qe)))
       poolInsert (tree, qe);

    maxMeasureQE (g, SYM (MAKEQE (i)), &qe, &exists);
    if (exists && (! poolDoesContain (tree, qe)))
       poolInsert (tree, qe);
  }
  
  return tree;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define SETqsIsNotVerified(x,y) \
  do {bvSet (qsIsNotVerified, (x) * qsIsNotVerifiedMaxIndex + y);} while (0)

#define UNSETqsIsNotVerified(x,y) \
  do {bvClear (qsIsNotVerified, (x) * qsIsNotVerifiedMaxIndex + y);} while (0)

#define TESTqsIsNotVerified(x,y) \
  (bvTest (qsIsNotVerified, (x) * qsIsNotVerifiedMaxIndex + y))

#define MAKEqsIsNotVerified(xMax) \
  do {qsIsNotVerifiedMaxIndex = xMax; \
      qsIsNotVerified = bvNew ((xMax+1) * (xMax+1), BV_FIXED_SIZE);} while (0)

#define DISPOSEqsIsNotVerified() \
  do {bvDispose (qsIsNotVerified);} while (0)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void
getMaxEdge (flipEdge, found)

     indexType *flipEdge;
     bool *found;

{
  indexType qe;

  *found = poolTop (edges, &qe);

  while (*found && ISFINALedge (g, QETOE( OPREV(g, SYM(qe))))) {
debugPrintQE (g, "isfinal:", MAKEQE (QETOE( OPREV(g, SYM(qe)))), 0);
    poolDelete (edges, qe);
    *found = poolTop (edges, &qe);
  }

  if (*found) {
    *flipEdge = SYM (OPREV (g, SYM(qe)));
  }
}

/*---------------------------------------------------------------------------*/

static void 
unCutEarP (prevToSQ, newPrevToSQ, finished)

     indexType prevToSQ;
     indexType * newPrevToSQ;
     bool *finished;

{
  indexType tmp;

  debugPrintQE (gPrime, "unCutEarP", prevToSQ, 6);

  *newPrevToSQ = OPREV (gPrime, SYM (prevToSQ));
  tmp = OPREV (gPrime, SYM (*newPrevToSQ));

  *finished = (DST (gPrime, tmp) != S);
  
  if (! *finished) {
    stkPush (edgeStack, tmp);
    /*(*visual->eraseEdge) (gPrime, visual, tmp);*/
    qeDelete (gPrime, tmp);
  }
}

/*---------------------------------------------------------------------------*/

static void 
unCutEarsP (prevToSQ)

     indexType prevToSQ;

{
  indexType newPrevToSQ;
  bool finished;

  do {

    debugPrintQE (gPrime, "unCutEarsP", prevToSQ, 5);
    unCutEarP (prevToSQ, &newPrevToSQ, &finished);
    prevToSQ = newPrevToSQ;
  
  } while (! finished);
}

/*---------------------------------------------------------------------------*/

static void 
unCutEarR (nextToSQ, newNextToSQ, finished)

     indexType nextToSQ;
     indexType * newNextToSQ;
     bool *finished;

{
   indexType tmp;

  debugPrintQE (gPrime, "unCutEarR", nextToSQ, 6); /*printf ("   %d\n", S);*/

  *newNextToSQ = ONEXT (gPrime, SYM (nextToSQ));
  tmp = ONEXT (gPrime, SYM (*newNextToSQ));

  *finished = (DST (gPrime, tmp) != S);

  debugPrintQE (gPrime, "unCutEarR_tmp", tmp, 7);
  
  if (! *finished) {
    stkPush (edgeStack, SYM (tmp));
    /*(*visual->eraseEdge) (gPrime, visual, tmp);*/
    qeDelete (gPrime, tmp);
  }
}

/*---------------------------------------------------------------------------*/

static void 
unCutEarsR (nextToSQ)

     indexType nextToSQ;

{
  indexType newNextToSQ;
  bool finished;

  do { 

    debugPrintQE (gPrime, "unCutEarsR", nextToSQ, 5);
    unCutEarR (nextToSQ, &newNextToSQ, &finished);
    nextToSQ = newNextToSQ;

  } while (! finished);
}

/*---------------------------------------------------------------------------*/

static void 
cutEarP (prevToSQ, newPrevToSQ, failed, done)

     indexType prevToSQ;
     indexType * newPrevToSQ;
     bool *failed, *done;

{
  criterionType newTriangle;
  indexType edge;
  indexType tmp;

  debugPrintQE (gPrime, "cutEarP", prevToSQ, 6);

  newTriangle.center = DST (gPrime, prevToSQ);
  newTriangle.left = ORG (gPrime, prevToSQ);
  tmp = OPREV (gPrime, SYM(prevToSQ));
  newTriangle.right = DST (gPrime, tmp);

  *failed = (*localGTfun) (gPrime, &newTriangle, &actMax);
  
  debugPrintQE (gPrime, "cutEarP_tmp", tmp, 7);
    
  if ((! *failed) && (newTriangle.right != Q)) {
    UNSETFINALQE (gPrime, prevToSQ);
    stkPop (edgeStack, &edge);
    /*(*visual->drawEdge) (gPrime, visual, tmp);*/
    qeReinsert (gPrime, SYM (edge), SYM (prevToSQ), OPREV (gPrime, SYM (tmp)));
  } else
    SETFINALQE (gPrime, prevToSQ);
  
  *done = ((! *failed) && (newTriangle.right == Q));

  *newPrevToSQ = (*done || *failed ? prevToSQ : ONEXT (gPrime, prevToSQ));
}

/*---------------------------------------------------------------------------*/

static void 
cutEarsP (prevToSQ, failed)

     indexType prevToSQ;
     bool * failed;

{
   bool done;
   indexType newPrevToSQ;

  do {

    debugPrintQE (gPrime, "cutEarsP", prevToSQ, 5);
    cutEarP (prevToSQ, &newPrevToSQ, failed, &done);
    prevToSQ = newPrevToSQ;

  } while (! (*failed || done));
}

/*---------------------------------------------------------------------------*/

static void 
cutEarR (prevToSQ, failed, done)

     indexType prevToSQ;
     bool *failed, *done;

{
   criterionType newTriangle;
   indexType edge;
   indexType tmp;

  debugPrintQE (gPrime, "cutEarR", prevToSQ, 6);

  tmp = ONEXT (gPrime, prevToSQ);
  newTriangle.center = DST (gPrime, tmp);
  newTriangle.right = ORG (gPrime, prevToSQ);
  tmp = ONEXT (gPrime, SYM (tmp));
  newTriangle.left = DST (gPrime, tmp);

  *failed = (*localGTfun) (gPrime, &newTriangle, &actMax);
  
  debugPrintQE (gPrime, "cutEarR_tmp", tmp, 7);
  
  if ((! *failed) && (newTriangle.left != Q)) {
    UNSETFINALQE (gPrime, ONEXT (gPrime, prevToSQ));
    stkPop (edgeStack, &edge);
    /*(*visual->drawEdge) (gPrime, visual, tmp);*/
    qeReinsert (gPrime, edge, SYM (prevToSQ), SYM (tmp));
  } else
    SETFINALQE (gPrime, ONEXT (gPrime, prevToSQ));
  
  *done = ((! *failed) && (newTriangle.left == Q));
}

/*---------------------------------------------------------------------------*/

static void 
cutEarsR (prevToSQ, failed)

     indexType prevToSQ;
     bool *failed;

{
  bool done;

  do {

    debugPrintQE (gPrime, "cutEarsR", prevToSQ, 5);
    cutEarR (prevToSQ, failed, &done);
  
  } while (! (*failed || done));
}

/*---------------------------------------------------------------------------*/

static void
cutEarsPandR (prevToSQ, failedLeft, failedRight)

     indexType prevToSQ;
     bool *failedLeft, *failedRight;
     
{
  indexType newPrevToSQ;
  bool doneLeft, doneRight;
  
  do {

    debugPrintQE (gPrime, "cutEarsPandR", prevToSQ, 4);

    cutEarR (prevToSQ, failedRight, &doneRight);
    cutEarP (prevToSQ, &newPrevToSQ, failedLeft, &doneLeft);
  
    prevToSQ = newPrevToSQ;

  } while (! (*failedLeft || *failedRight || doneLeft || doneRight));

  if (! (*failedLeft || *failedRight))
    if (doneLeft && (! doneRight)) 
      cutEarsR (prevToSQ, failedRight);
    else
      if (doneRight && (! doneLeft))
	cutEarsP (prevToSQ, failedLeft);
}

/*---------------------------------------------------------------------------*/

static void 
pushEarsP (prevToSQ)

     indexType prevToSQ;

{
  indexType count = 0;

  debugPrintQE (gPrime, "pushEarsP", prevToSQ, 6);

  while (! ISFINALQE (gPrime, prevToSQ)) {
    prevToSQ = ONEXT (gPrime, prevToSQ);
    count++;
  }

  while (count) {
  debugPrintQE (gPrime, "pushing Ear P", SYM(prevToSQ), 6);
    stkPush (edgeStack, SYM (prevToSQ));
    prevToSQ = OPREV (gPrime, prevToSQ);
    count--;
  }
}

/*---------------------------------------------------------------------------*/

static void 
pushEarsR (nextToSQ)

     indexType nextToSQ;

{
  indexType count = 0;
  
  debugPrintQE (gPrime, "pushEarsR", nextToSQ, 6);
  
  while (! ISFINALQE (gPrime, nextToSQ)) {
    nextToSQ = OPREV (gPrime, nextToSQ);
    count++;
  }
  
  while (count) {
  debugPrintQE (gPrime, "pushing Ear R", nextToSQ, 6);
    stkPush (edgeStack, nextToSQ);
    nextToSQ = ONEXT (gPrime, nextToSQ);
    count--;
  }
}

/*---------------------------------------------------------------------------*/

static bool
findS (PR)

     indexType PR;

{
  indexType prevToSQ, nextToSQ;
  static bool failed, failedLeft, failedRight, isInsideLeft, isInsideRight;
  static indexType tmp;
  static bool dummy, existsPfan, existsRfan;

 debugPrintQE (gPrime, "FindS", PR, 3); /*printf ("                %d\n", Q);*/

  if (ISFINALQE (g, PR))
    failed = 1;
  else {
    prevToSQ = SYM (ONEXT (g, PR));
    nextToSQ = ONEXT (g, prevToSQ); /* == SYM (OPREV (g, SYM (PR))) */
    S = ORG (g, prevToSQ); /* == third */

    if (ISDELETEDedge (gPrime, QETOE (prevToSQ))) {
      qeDuplicate (g, gPrime, prevToSQ);
      UNSETDELETEDedge (gPrime, QETOE(prevToSQ));
    }
    if (ISDELETEDedge (gPrime, QETOE(nextToSQ))) {
      qeDuplicate (g, gPrime, nextToSQ);
      UNSETDELETEDedge (gPrime, QETOE(nextToSQ));
    }

    qeDelete(gPrime, PR);
    
    stkPush (edgeStack, PR);
    (flips)++;

    (*visual->eraseEdge) (g, visual, PR);
INTERRUPT_ALGORITHM;
    
    testInsideWedge (g, S, &wedge, &isInsideRight, &isInsideLeft);
    
    if (isInsideRight && isInsideLeft) {
      cutEarsPandR (prevToSQ, &failedLeft, &failedRight);
      failed = (failedLeft && failedRight);
      
      if (! failed)
	if (failedLeft) {
	  unCutEarsR (nextToSQ);
	  wedge.left = S;
	  failed = ! findS (nextToSQ);
INTERRUPT_ALGORITHM;
	  existsPfan = 1;
	  existsRfan = 0;
	} else 
	  if (failedRight) {
	    unCutEarsP (prevToSQ);
	    wedge.right = S;
	    failed = ! findS (SYM (prevToSQ));
INTERRUPT_ALGORITHM;
	    existsPfan = 0;
	    existsRfan = 1;
	  } else {
	    stkPop (edgeStack, &tmp);
	    qeReinsert (gPrime, tmp, SYM (OPREV (gPrime, SYM (prevToOrigPR))),
			SYM (ONEXT (gPrime, prevToOrigPR)));
	    if (! stkEmpty (edgeStack))
	      printf ("ERROR: edgeinsert: finds: not stkEmpty!\n");
	    stkPush (edgeStack, tmp);
	    if (poolDoesContain (edges, prevToSQ))
	      poolDelete (edges, prevToSQ);
	    existsPfan = 1;
	    existsRfan = 1;
	  }
    } else
      if (! isInsideLeft) {
	cutEarsP (prevToSQ, &dummy);
	failed = ! findS (nextToSQ);
INTERRUPT_ALGORITHM;
	existsPfan = 1;
	existsRfan = 0;
      } else {
	cutEarsR (prevToSQ, &dummy);
	failed = ! findS (SYM (prevToSQ));
INTERRUPT_ALGORITHM;
	existsPfan = 0;
	existsRfan = 1;
      }
      
    if (! failed) {
      if (poolDoesContain (edges, PR))
	poolDelete (edges, PR);
      if (poolDoesContain (edges, SYM (PR)))
	poolDelete (edges, SYM (PR));
      if (poolDoesContain (edges, SYM (nextToSQ)))
	poolDelete (edges, SYM (nextToSQ));
      if (poolDoesContain (edges, OPREV (g, PR)))
	poolDelete (edges, OPREV (g, PR));

      if (existsRfan)
	pushEarsR (nextToSQ);
      if (existsPfan)
	pushEarsP (prevToSQ);
      
      qeDelete(g, PR);
    } else {
      (*visual->drawEdge) (g, visual, PR, 1);
INTERRUPT_ALGORITHM;
    }      

    SETDELETEDedge (gPrime, QETOE(prevToSQ));
    SETDELETEDedge (gPrime, QETOE(nextToSQ));
  }

INTERRUPT_LABEL: 
  return ! failed;
}

/*---------------------------------------------------------------------------*/

static bool
verifyS (PR)

     indexType PR;

{
  indexType prevToSQ, nextToSQ;
  static bool failed, failedLeft, failedRight, isInsideLeft, isInsideRight;
  static indexType tmp;
  static bool dummy, existsPfan, existsRfan;

debugPrintQE (gPrime, "verifyS", PR, 3); /*printf ("                %d\n", Q);*/

  if (ISFINALQE (g, PR))
    failed = 1;
  else {
    prevToSQ = SYM (ONEXT (g, PR));
    nextToSQ = ONEXT (g, prevToSQ); /* == SYM (OPREV (g, SYM (PR))) */
    S = ORG (g, prevToSQ); /* == third */
    if (ISDELETEDedge (gPrime, QETOE (prevToSQ))) {
      qeDuplicate (g, gPrime, prevToSQ);
      UNSETDELETEDedge (gPrime, QETOE(prevToSQ));
    }
    if (ISDELETEDedge (gPrime, QETOE(nextToSQ))) {
      qeDuplicate (g, gPrime, nextToSQ);
      UNSETDELETEDedge (gPrime, QETOE(nextToSQ));
    }

    qeDelete(gPrime, PR);
    
    stkPush (edgeStack, PR);
    (flips)++;

    (*visual->eraseEdge) (g, visual, PR);
INTERRUPT_ALGORITHM;

    testInsideWedge (g, S, &wedge, &isInsideRight, &isInsideLeft);

    if (isInsideRight && isInsideLeft) {
      cutEarsPandR (prevToSQ, &failedLeft, &failedRight);
      if (failedLeft || failedRight)  {
	UNSETqsIsNotVerified (Q, S);
	if (S != globS)
	  if (isLeft ()) {
	    debugPrintQE (gPrime, "verifying", prevToSQ, 3);	
	    unCutEarsP (prevToSQ);
	    cutEarsR (prevToSQ, &failedRight);
	    wedge.right = S;
	    failed = ! verifyS (SYM (prevToSQ));
	    existsPfan = 0;
	    existsRfan = 1;
	  } else {
	    debugPrintQE (gPrime, "verifying", nextToSQ, 3);	
	    unCutEarsR (nextToSQ);
	    cutEarsP (OPREV (gPrime, nextToSQ), &failedLeft);
	    wedge.left = S;
	    failed = ! verifyS (nextToSQ);
	    existsPfan = 1;
	    existsRfan = 0;
	  }
	else
	  failed = 1;
      } else {
	failed = 0;
	stkPop (edgeStack, &tmp);
	qeReinsert (gPrime, tmp, SYM (OPREV (gPrime, SYM (prevToOrigPR))),
		    SYM (ONEXT (gPrime, prevToOrigPR)));
	if (! stkEmpty (edgeStack))
	  printf ("ERROR: edgeinsert: verifyS: not stkEmpty!\n");
	stkPush (edgeStack, tmp);
	if (poolDoesContain (edges, prevToSQ))
	  poolDelete (edges, prevToSQ);
	existsPfan = 1;
	existsRfan = 1;
      }
    } else
      if (S != globS)
	if (! isInsideLeft) {
	  cutEarsP (prevToSQ, &dummy);
	  failed = ! verifyS (nextToSQ);
	  existsPfan = 1;
	  existsRfan = 0;
	} else {
	  cutEarsR (prevToSQ, &dummy);
	  failed = ! verifyS (SYM (prevToSQ));
	  existsPfan = 0;
	  existsRfan = 1;
	}
      else 
	failed = 1;
    
    if (! failed) {
      if (poolDoesContain (edges, PR))
	poolDelete (edges, PR);
      if (poolDoesContain (edges, SYM (PR)))
	poolDelete (edges, SYM (PR));
      if (poolDoesContain (edges, SYM (nextToSQ)))
	poolDelete (edges, SYM (nextToSQ));
      if (poolDoesContain (edges, OPREV (g, PR)))
	poolDelete (edges, OPREV (g, PR));
      
      if (existsRfan)
	pushEarsR (nextToSQ);
      if (existsPfan)
	pushEarsP (prevToSQ);
      
      qeDelete(g, PR);
    } else {
      (*visual->drawEdge) (g, visual, PR, 1);
INTERRUPT_ALGORITHM;
    }

    SETDELETEDedge (gPrime, QETOE(prevToSQ));
    SETDELETEDedge (gPrime, QETOE(nextToSQ));
  }

INTERRUPT_LABEL: ;	
  return ! failed;
}

/*---------------------------------------------------------------------------*/

static bool
findSslow (PR)

     indexType PR;

{
  static bool succeeded, isInsideLeft, isInsideRight;


  UNSETqsIsNotVerified (Q, wedge.center);
  globS = 0;
  succeeded = 0;

  do {
    if (TESTqsIsNotVerified (Q, globS)) {
      testInsideWedge (g, globS, &wedge, &isInsideRight, &isInsideLeft);
      if (isInsideRight && isInsideLeft && (globS != wedge.left) &&
	  (globS != wedge.right)) {

	succeeded = verifyS (PR);
INTERRUPT_ALGORITHM;
	if (! succeeded) {
	  wedge.center = Q;
	  wedge.left = ORG (g, PR);
	  wedge.right = DST (g, PR);
	  stkReset (edgeStack);
	  qeDuplicate (g, gPrime, PR);
	  UNSETDELETEDedge (gPrime, QETOE(PR));
	  qeDuplicate (g, gPrime, prevToOrigPR);
	  UNSETDELETEDedge (gPrime, QETOE(prevToOrigPR));
	  qeDuplicate (g, gPrime, symNextSymToPR);
	  UNSETDELETEDedge (gPrime, QETOE(symNextSymToPR));
	}
	UNSETqsIsNotVerified (Q, wedge.center);

      }
    }
    globS++;
  } while ((! succeeded) && (globS < NS(g)));

INTERRUPT_LABEL: ;
  return succeeded;
}

/*---------------------------------------------------------------------------*/

static void 
initializeQsIsNotVerified ()

{
  int i, j;

  MAKEqsIsNotVerified (NS(g));
  
  for (i = 0; i <= NS(g); i++)
    for (j = 0; j <= NS(g); j++)
      SETqsIsNotVerified (i, j);
}

/*---------------------------------------------------------------------------*/

static void 
freeQsIsNotVerified ()

{
 DISPOSEqsIsNotVerified ();
}

/*---------------------------------------------------------------------------*/

static void
findQS (PR, modus)

     indexType PR;
     int       modus;

{
  bool found;
  indexType qe;
  indexType maxQE;
  int existsMaxQE;

  prevToOrigPR = OPREV (g, PR);
  symNextSymToPR = SYM (ONEXT (g, SYM (PR)));
  
  Q = DST (g, prevToOrigPR);

  actMax.center = Q;
  actMax.left = ORG (g, PR);
  actMax.right = DST (g, PR);

  wedge.center = Q;
  wedge.left = ORG (g, PR);
  wedge.right = DST (g, PR);

  stkReset (edgeStack);

  poolDelete (edges, symNextSymToPR);

  qeDuplicate (g, gPrime, PR);
  UNSETDELETEDedge (gPrime, QETOE(PR));
  qeDuplicate (g, gPrime, prevToOrigPR);
  UNSETDELETEDedge (gPrime, QETOE(prevToOrigPR));
  qeDuplicate (g, gPrime, symNextSymToPR);
  UNSETDELETEDedge (gPrime, QETOE(symNextSymToPR));

  debugPrintQE (gPrime, "FindQS", PR, 2);
/*  printEdgeMeasure (g, symNextSymToPR);*/

  flips = 0;

  found = (modus == N2LOGN ? findS (PR) : findSslow (PR));
INTERRUPT_ALGORITHM;

  SETDELETEDedge (gPrime, QETOE(PR));
  SETDELETEDedge (gPrime, QETOE(prevToOrigPR));
  SETDELETEDedge (gPrime, QETOE(symNextSymToPR));


  if (! found) {
    debugPrintQE (gPrime, "finalizing", PR, 0);
    debugPrintQE (gPrime, "finalizing", prevToOrigPR, 0);
    debugPrintQE (gPrime, "finalizing", symNextSymToPR, 0);

    SETFINALedge (g, QETOE (PR));
    SETFINALedge (g, QETOE (prevToOrigPR));
    SETFINALedge (g, QETOE (symNextSymToPR));
    attempts = flips;
    flips = 0;
  } else {
    do {
      stkPop (edgeStack, &qe);

  debugPrintQE (gPrime, "before", qe, 3);
  debugPrintQE (gPrime, "onext", ONEXT (gPrime, qe), 3);
  debugPrintQE (gPrime, "oprev", OPREV (gPrime, qe), 3);
  debugPrintQE (gPrime, "onext_sym", ONEXT (gPrime, SYM (qe)), 3);
  debugPrintQE (gPrime, "oprev_sym", OPREV (gPrime, SYM (qe)), 3);

      qeReinsert (g, qe, SYM (OPREV (g, ONEXT (gPrime, qe))),
		  OPREV (gPrime, SYM (qe)));

      (*visual->drawEdge) (g, visual, qe, 1);
INTERRUPT_ALGORITHM;

  debugPrintQE (g, "after", qe, 3);

      maxMeasureQE (g, qe, &maxQE, &existsMaxQE);
      if (existsMaxQE)
	poolInsert (edges, maxQE);

    } while (! stkEmpty (edgeStack));

    maxMeasureQE (g, symNextSymToPR, &maxQE, &existsMaxQE);
    if (existsMaxQE)
      poolInsert (edges, maxQE);
    attempts = 0;
  }

INTERRUPT_LABEL: ;
}

/*---------------------------------------------------------------------------*/

static void
doEdgeInsertion (modus, local_g, local_visual, local_pool,
		 local_applicationGT, GTdecidesRightTurn,
		 local_applicationRightTurn,
		 local_applicationPrintMeasure,
		 nofFlips, nofAttempts, runTime) 

     int        GTdecidesRightTurn;
     int        modus;
     graphType  *local_g;
     visualType *local_visual;
     poolType   *local_pool;
     bool       (*local_applicationGT) ();
     bool       (*local_applicationRightTurn) ();
     void       (*local_applicationPrintMeasure) ();
     int        *nofFlips, *nofAttempts;
     double * runTime;
     
{
  indexType newPR;
  bool found;
  
  g = local_g;
  visual = local_visual;
  pool = local_pool;
  applicationGT = local_applicationGT;
  applicationRightTurn = local_applicationRightTurn;
  applicationPrintMeasure = local_applicationPrintMeasure;

  if (GTdecidesRightTurn == GT_CAN_DECIDE_RIGHT_TURN)
    localGTfun = localGT;
  else 
    localGTfun = localGTplusRightTurn;
  
  *runTime = get_user_time();
  markFinalEdges (g);
  edges = initializePool (g, pool);
  edgeStack = stkNew(2*NE(g));
  mkCopyOfG (g, &gPrime);
  if (modus == N3)
    initializeQsIsNotVerified ();
  
  *nofFlips = 0;
  *nofAttempts = 0;

  getMaxEdge (&newPR, &found);

  while (found) {
    findQS(newPR, modus);
INTERRUPT_ALGORITHM;   

    *nofFlips += flips;
    *nofAttempts += attempts;

    getMaxEdge (&newPR, &found);
  }

INTERRUPT_LABEL:
  stkDispose (edgeStack);
  poolDispose (edges);
  free ((char *) gPrime->e);
  free ((char *) gPrime);
  unmarkFinalEdges (g);
  if (modus == N3)
    freeQsIsNotVerified ();
if (! SHOULD_INTERRUPT_ALGORITHM) {
  *runTime = get_user_time() - *runTime;
  printf ("  completed, cpu used ... %f.\n", *runTime);
  printf ("                  # flips ... %d, #attempts %d.\n\n",
	  *nofFlips, *nofAttempts);
}
} 

/*---------------------------------------------------------------------------*/

void
edgeInsertionN3 (local_g, local_visual, local_pool,
		 local_applicationGT, GTdecidesRightTurn,
		 local_applicationRightTurn,
		 local_applicationPrintMeasure,
		 nofFlips, nofAttempts, runTime) 

     int        GTdecidesRightTurn;
     graphType  *local_g;
     visualType *local_visual;
     poolType   *local_pool;
     bool       (*local_applicationGT) ();
     bool       (*local_applicationRightTurn) ();
     void       (*local_applicationPrintMeasure) ();
     int        *nofFlips, *nofAttempts;
     double * runTime;
     
{
  printf("Edge Insertion (n^3): ");  (void) fflush(stdout);

  doEdgeInsertion (N3, local_g, local_visual, local_pool,
		   local_applicationGT, GTdecidesRightTurn,
		   local_applicationRightTurn,
		   local_applicationPrintMeasure,
		   nofFlips, nofAttempts, runTime); 
}

/*---------------------------------------------------------------------------*/

void
edgeInsertionN2LOGN (local_g, local_visual, local_pool,
		     local_applicationGT, GTdecidesRightTurn,
		     local_applicationRightTurn,
		     local_applicationPrintMeasure,
		     nofFlips, nofAttempts, runTime)   

     int        GTdecidesRightTurn;
     graphType  *local_g;
     visualType *local_visual;
     poolType   *local_pool;
     bool       (*local_applicationGT) ();
     bool       (*local_applicationRightTurn) ();
     void       (*local_applicationPrintMeasure) ();

     int        *nofFlips, *nofAttempts;
     double * runTime;
     
{
  printf("Edge Insertion (n^2 log n): ");  (void) fflush(stdout);

  doEdgeInsertion (N2LOGN, local_g, local_visual, local_pool, 
		   local_applicationGT, GTdecidesRightTurn,
		   local_applicationRightTurn, 
		   local_applicationPrintMeasure,
		   nofFlips, nofAttempts, runTime); 
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
