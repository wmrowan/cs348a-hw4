/*
 * NAME: persistent.quadedge.c
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>
#include "road.h"
#include "quadedge.h"
#include "graph.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ERRFILE stdout
pqeDebug = 0;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define MOD4(n) ((n) & 3)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* PRIVATE FUNCTIONS */
/* these functions do not increase the time */

/*---------------------------------------------------------------------------*/

int
countNofNext (pq)
     
     persistQeType * pq;

{
  int count;
  additionalNextRecordsType * anr;
  indexType i;

  count = 0;
  
  i = 0;
  while ((i < NOF_NEXT) && (pq->nxt[i].time != MARKED_LAST)) {
    count++;
    i++;
  }

  anr = pq->additional;

  while (anr != NULL) {

    i = 0;
    while ((i < NOF_NEXT_ADDITIONAL) && (anr->nxt[i].time != MARKED_LAST)) {
      count++;
      i++;
    }
    anr = anr->additional;
  }

  return count;
}

/*---------------------------------------------------------------------------*/

void
printPersistQe (pq, index)
     
     persistQeType * pq;
     indexType index;

{
  additionalNextRecordsType * anr;
  indexType i;

  printf ("\npq (%d): dat = %d, additional = %d\n", index, pq->dat, pq->additional);
  
  i = 0;
  while ((i < NOF_NEXT) && (pq->nxt[i].time != MARKED_LAST)) {
    printf ("         time: %d, edge: %d\n", pq->nxt[i].time, pq->nxt[i].edge);
    i++;
  }

  if ((i < NOF_NEXT) && (pq->nxt[i].time == MARKED_LAST))
    printf ("         time: last, edge: %d\n", pq->nxt[i].edge);

  printf ("\n");

  anr = pq->additional;

  while (anr != NULL) {
    printf ("  anr->additional = %d\n", anr->additional);
    i = 0;
    while ((i < NOF_NEXT_ADDITIONAL) && (anr->nxt[i].time != MARKED_LAST)) {
      printf ("         time: %d, edge: %d\n", anr->nxt[i].time,
	      anr->nxt[i].edge);
      i++;
    }
    if ((i < NOF_NEXT_ADDITIONAL) && (anr->nxt[i].time == MARKED_LAST))
      printf ("         time: last, edge: %d\n", anr->nxt[i].edge);
    anr = anr->additional;
  }
}

/*---------------------------------------------------------------------------*/

void
printPersistQe2 (nxt, index, maxEntries, additional)
     
     nextRecordType *nxt;
     indexType index;
     indexType maxEntries;
     additionalNextRecordsType *additional;


{
  additionalNextRecordsType * anr;
  indexType i;

  printf ("\Fast: index = %d, maxEntries = %d\n", index, maxEntries);
  
  i = 0;
  while ((i <= maxEntries) && (nxt[i].time != MARKED_LAST)) {
    printf ("         time: %d, edge: %d\n", nxt[i].time, nxt[i].edge);
    i++;
  }

  if ((i < maxEntries) && (nxt[i].time == MARKED_LAST))
    printf ("         time: last, edge: %d\n", nxt[i].edge);

  printf ("\n");

  anr = additional;

  while (anr != NULL) {
    printf ("  anr->additional = %d\n", additional);
    i = 0;
    while ((i <= NOF_NEXT_ADDITIONAL) && (anr->nxt[i].time != MARKED_LAST)) {
      printf ("         time: %d, edge: %d\n", anr->nxt[i].time,
	      anr->nxt[i].edge);
      i++;
    }
    if ((i < NOF_NEXT_ADDITIONAL) && (anr->nxt[i].time == MARKED_LAST))
      printf ("         time: last, edge: %d\n", anr->nxt[i].edge);
    anr = anr->additional;
  }
}

/*---------------------------------------------------------------------------*/

void
printPQ (g)

     graphType *g;

{
  indexType index;

  printf ("\n---------------------\n");

  for (index = 0; index < NE(g) * 2; index++) {
    printPersistQe (&(g->pe[index * 2]), index * 2);
  }
}

/*---------------------------------------------------------------------------*/

additionalNextRecordsType *
makeNewNextRecord ()

{
  additionalNextRecordsType * tmp;

  tmp = (additionalNextRecordsType *) 
    malloc ((unsigned int) sizeof (additionalNextRecordsType));

  if (tmp == NULL) {
    printf("makeNewNextRecord: fail on additionalNextRecords allocation.\n");
    exit (1);
  }

  tmp->additional = NULL;
  tmp->nxt[0].time = MARKED_LAST;

  return tmp;
}

/*---------------------------------------------------------------------------*/

static void
pSETONEXTintern (g, qe, next)

     graphType *g;
     indexType qe, next;

{
  nextRecordType *nxt;
  indexType index;
  indexType maxEntries;
  additionalNextRecordsType **additional;

  pqeGetPresent (g, qe, TIME_NOW, &nxt, &index, &maxEntries, &additional);

/*  printf ("pSETONEXTintern: %d %d %d %d\n", qe, next, index, maxEntries);*/

  if (nxt[index].time == g->time)
    nxt[index].edge = next;
  else
    if (nxt[index].time == MARKED_LAST) {
      nxt[index].edge = next;
      nxt[index].time = g->time;
      if (index <  maxEntries - 1)
        nxt[index + 1].time = MARKED_LAST;
    } else
      if (index < maxEntries - 1) {
	nxt[index + 1].edge = next;
	nxt[index + 1].time = g->time;
	if (index < maxEntries - 2)
	  nxt[index + 2].time = MARKED_LAST;
      } else {
	*additional = makeNewNextRecord ();
	(*additional)->nxt[0].edge = next;
	(*additional)->nxt[0].time = g->time;
	if (1 != NOF_NEXT_ADDITIONAL)
	  (*additional)->nxt[1].time = MARKED_LAST;
      }
}

/*---------------------------------------------------------------------------*/
static void
pMarkDeletedEdgeIntern (g, edge)

     graphType *g;
     indexType edge;

{
  indexType qe;

  qe = pMAKEQE (edge);
  pSETONEXTintern (g, qe, MARKED_DELETED);
  pSETONEXTintern (g, pSYM(qe), MARKED_DELETED);
  pSETONEXTintern (g, pROT1(qe), MARKED_DELETED);
  pSETONEXTintern (g, pROT3(qe), MARKED_DELETED);

}

/*---------------------------------------------------------------------------*/

static indexType
MakeNewEdge (g)

     graphType *g;

{
  indexType qe;

  if (NE (g) == g->maxe) {
    g->pe = (persistQeType *) realloc((char *) g->pe, 
				     (unsigned int) (1.2 * (float) g->maxe) * 
				     4 * sizeof (persistQeType));
    if (g->pe == NULL) {
      printf("Reallocate: fail on quadedge re-allocation.\n");
      exit (1);
    } 
    
    g->maxe *= 1.2;
  }

  qe = pMAKEQE (NE (g));
  g->pe[qe].nxt[0].time = MARKED_LAST;
  g->pe[qe].additional = NULL;

  qe = SYM (qe);
  g->pe[qe].nxt[0].time = MARKED_LAST;
  g->pe[qe].additional = NULL;


  qe = ROT1 (qe);
  g->pe[qe].nxt[0].time = MARKED_LAST;
  g->pe[qe].additional = NULL;

  qe = SYM (qe);
  g->pe[qe].nxt[0].time = MARKED_LAST;
  g->pe[qe].additional = NULL;

  NE (g) += 1;
  return pMAKEQE (NE (g) - 1);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static indexType 
pqeAddSiteSiteIntern (g, s1, s2)

     graphType *g;
     indexType s1, s2;	/* the two sites where the edge will be added */

/*   qeAddSiteSite adds an edge from site s1 to site s2, assuming that */
/*   neither site already has an edge connected to it. */ 

{
  indexType qenew;

  if (pqeDebug) 
    (void) printf (
      "qeAddSiteSite: adding edge %d from %d to %d\n", NE (g), s1, s2);
  
  qenew = MakeNewEdge (g);

  if (((s1 == 46) && (s2 == 108)) || ((s1 == 108) && (s2 == 46)))
    printf ("pqeAddSiteSiteIntern %d\n", qenew);

  pSETORG (g, qenew, s1);
  pSETDST (g, qenew, s2);

  pSETONEXTintern (g, qenew, qenew);
  pSETONEXTintern (g, qenew +1, pROT3 (qenew));
  pSETONEXTintern (g, qenew +2, pSYM (qenew));
  pSETONEXTintern (g, qenew +3, pROT1 (qenew));

  return (qenew);
}

/*---------------------------------------------------------------------------*/

static indexType 
pqeAddEdgeSiteIntern (g, a, s)

     graphType *g;
     indexType a;		/* quadedge pointing to origin of new site */
     indexType s;		/* site which will be new edge's destination */
/*
 *   qeAddSite adds an edge from the Dest of edge "a" to site s.  It
 * is assumed that site s has no other edges connected to it.  Furthermore,
 * the following equalities must hold: aRight == qenewRight and 
 * aRnext == qenewOnext, where qenew is the new edge added.
 *
 *  aRnext \
 *          *- - - - ->* s
 *          ^   qenew
 *          | a
 */
{
  indexType qenew, aRnext;

  aRnext = pONEXT (g, pSYM (a), (indexType) g->time - 1);

  if (pqeDebug) 
    (void) printf ("qeAddEdgeSite: adding edge %d from site %d to %d\n", 
      NE (g), DST (g, a), s);

  qenew = MakeNewEdge (g);

  pSETORG (g, qenew, pDST (g, a));
  pSETORG (g, pSYM (qenew), s);

  pSETONEXTintern (g, qenew, aRnext);
  pSETONEXTintern (g, qenew +1, pROT1 (a));
  pSETONEXTintern (g, qenew +2, pSYM (qenew));
  pSETONEXTintern (g, qenew +3, pROT1 (qenew));

  pSETONEXTintern (g, pSYM (a), qenew);
  pSETONEXTintern (g, pROT1 (aRnext), pROT3 (qenew));

  return (qenew);
}

/*---------------------------------------------------------------------------*/

indexType 
pqeAddEdgeEdgeIntern (g, a, b)

     graphType *g;
     indexType a, b;

/*
 *   qeAddEdgeEdge adds an edge to between the site pointed to by edge
 * a and the site which is the origin of edge b.  The following equalities
 * must to hold: aRight == qenewRight and qenewLeft == bLeft.
 *
 *    \ aRnext       ^
 *     \    qenew    | b
 *      *- - - - - ->*
 *      ^             \
 *      |a             \ bLprev
 */
{
  indexType qenew, aRnext, bLprev;

  aRnext = pONEXT (g, pSYM (a), (indexType) g->time - 1);
  bLprev = pSYM (pONEXT (g, b, (indexType) g->time - 1));

  if (pqeDebug) 
    (void) printf ("qeAddEdgeEdge: adding edge %d from site %d to %d\n", 
      NE (g), DST (g, a), ORG (g, b));

  qenew = MakeNewEdge (g);

  pSETORG (g, qenew, pDST (g, a));
  pSETORG (g, pSYM (qenew), pORG (g, b));

  pSETONEXTintern (g, qenew, aRnext);
  pSETONEXTintern (g, qenew +1, pROT1 (a));
  pSETONEXTintern (g, qenew +2, pSYM (bLprev));
  pSETONEXTintern (g, qenew +3, pROT3 (b));

  pSETONEXTintern (g, pSYM (a), qenew);
  pSETONEXTintern (g, b, pSYM (qenew));
  pSETONEXTintern (g, pROT1 (aRnext), pROT3 (qenew));
  pSETONEXTintern (g, pROT3 (bLprev), pROT1 (qenew));

  return (qenew);
} 

/*---------------------------------------------------------------------------*/

static char *
ename (qe)

     indexType qe;

{
  static char *mod[] = { "", "Rot", "Sym", "pROT3" };
  static char *undef = { "undefined" };
  static char answer[8];

  if (qe < 0) return (undef);

  (void) sprintf (answer, "e%d %s", QETOE (qe), mod[QNBR (qe)]);
  return (answer);
}

/*---------------------------------------------------------------------------*/

indexType
pqeFlipIntern (g, qe)

     graphType *g;
     indexType qe;

/*            *                          *
 * eSymOprev /^\ eSymOnext             /   \
 *          / | \                     / e'  \
 *         * e|  *          ===>     *<------*
 *   eOnext \ | / eOprev              \     /
 *           \|/                       \   /
 *            *                          *
 */

/* there is an important differnce between this version and the non */
/* persistent version: here we cannot use 'e' for the flipped edge, */
/* since e still has to remain to exist for older versions, so we have */
/* to make a new edge e' */

{
  indexType eOnext, eOprev, eSymOnext, eSymOprev;
  indexType qenew, time;

  if (pqeDebug) (void) printf ("qeFlip: flipping edge %s\n", ename (qe));

  time = g->time - 1;

  /* save neighboring edges in original configuration (in this */
  /* persistant version this is only a speed-up) */
  eOnext = pONEXT (g, qe, time);
  eOprev = pOPREV (g, qe, time);
  eSymOnext = pONEXT (g, pSYM (qe), time);
  eSymOprev = pOPREV (g, pSYM (qe), time);
  
  qenew = MakeNewEdge (g);

  /* set delete time for qe */
  pMarkDeletedEdgeIntern (g, pQETOE (qe));

  /* set sites to new values */
  pSETORG (g, qenew, pDST (g, pONEXT (g, pSYM (qe), time)));
  pSETORG (g, pSYM (qenew), pDST (g, pONEXT (g, qe, time)));

  /* set nxt pointers of the flipped edge */
  pSETONEXTintern (g, qenew, pSYM (eOprev));
  pSETONEXTintern (g, pROT1 (qenew), pROT1 (eSymOnext));
  pSETONEXTintern (g, pSYM (qenew), pSYM (eSymOprev));
  pSETONEXTintern (g, pROT3 (qenew), pROT1 (eOnext));

  /* Update pointers of neighboring edges. */
  pSETONEXTintern (g, pROT1 (eOnext), pROT3 (eOprev));
  pSETONEXTintern (g, pSYM (eOnext), pSYM (qenew));

  pSETONEXTintern (g, eOprev, eOnext);
  pSETONEXTintern (g, pROT3 (eOprev), pROT3 (qenew));

  pSETONEXTintern (g, pROT1 (eSymOnext), pROT3 (eSymOprev));
  pSETONEXTintern (g, pSYM (eSymOnext), qenew);

  pSETONEXTintern (g, eSymOprev, eSymOnext);
  pSETONEXTintern (g, pROT3 (eSymOprev), pROT1 (qenew));

  return qenew;
}

/*---------------------------------------------------------------------------*/

static void 
pqeDeleteIntern (g, qe)

     graphType *g;
     indexType qe;

/*            *                         *
 * eSymOprev /^\ eSymOnext             / \
 *          / | \                     /   \
 *         * e|  *          ===>     *     * 
 *   eOnext \ | / eOprev              \   /  
 *           \|/                       \ /   
 *            *                         *    
 */

{
  indexType eOnext, eOprev, eSymOnext, eSymOprev;
  indexType time;

  if (pqeDebug) (void) printf("pqeDeleteIntern: flipping edge %s\n", ename(qe));

  time = g->time - 1;

  /* set delete time for qe */
  pMarkDeletedEdgeIntern (g, pQETOE (qe));

  /* save neighboring edges in original configuration */
  eOnext = pONEXT(g, qe, time);
  eOprev = pOPREV(g, qe, time);
  eSymOnext = pONEXT(g, pSYM(qe), time);
  eSymOprev = pOPREV(g, pSYM(qe), time);

  /* Update pointers of neighboring edges. */
  pSETONEXTintern (g, pROT1(eOnext), pROT3(eOprev));
  pSETONEXTintern (g, eOprev, eOnext);
  pSETONEXTintern (g, pROT1(eSymOnext), pROT3(eSymOprev));
  pSETONEXTintern (g, eSymOprev, eSymOnext);
}

/*---------------------------------------------------------------------------*/

static indexType
pqeAddTriangleStarIntern (g, qe, p)

     graphType *g;
     indexType qe;
     indexType p;

/*                 qe                              qe
             *<------------*               a *<------------* b
              \           /                   \ \       / /     
               \    p    /                     \  \ p /  /      
                \   *   /         ====>         \   *   /       
              e  \     / f                     e \  |  / f       
                  \   /                           \ | /         
                   \ /                             \|/          
                    *                               * c   

    where initially f == pONEXT (qe) and e == pOPREV (pSYM (qe)) and
                    e == pSYM (pONEXT (pSYM (f))).
*/

/* the return value is the newly inserted quadedge from the origin of */
/* qe to p. */

{
  indexType qeSym, bp, pa;

  qeSym = pSYM (qe);

  /* insert quadedge b -> p */
  bp = pqeAddEdgeSiteIntern (g, qeSym, p);
  
  /* insert quadedge p -> a */
  pa = pqeAddEdgeEdgeIntern (g, bp, pOPREV (g, qeSym, g->time - 1));
  
  /* insert quadedge c -> p */
  (void) pqeAddEdgeEdgeIntern (g, pONEXT (g, bp, g->time), pa);

  return bp;
}

/*---------------------------------------------------------------------------*/

static void
pqeDeleteTriangleStarIntern (g, qe)

     graphType *g;
     indexType qe;

/*                                          
           qe                                qe                      
   a *<------------* b                 *<------------*                
      \ \       / /                     \           /                 
       \  \ p /  /                       \    p    /    
        \   *   /          ====>          \   *   /                    
       e \  |  / f                      e  \     / f                  
          \ | /                             \   /                     
           \|/                               \ /                     
            * c        	                      *         
*/

{
  indexType bp;
  indexType time;
  indexType eOnext, eSymOnext, eSymOprev, eOPPOSITE;

  time = g->time - 1;
  bp = pONEXT (g, qe, time);

  /* save neighboring edges in original configuration */
  eOnext = pONEXT(g, bp, time);
  eSymOnext = pONEXT(g, pSYM(bp), time);
  eSymOprev = pOPREV(g, pSYM(bp), time);
  eOPPOSITE = pSYM (pONEXT (g, pONEXT (g, pSYM (eOnext), time), time));

  /* delete edges incident to p */
  pMarkDeletedEdgeIntern (g, pQETOE (bp));
  pMarkDeletedEdgeIntern (g, pQETOE (eSymOnext));
  pMarkDeletedEdgeIntern (g, pQETOE (eSymOprev));

  /* Update pointers of neighboring edges. */
  pSETONEXTintern (g, pROT1(eOnext), pROT3(qe));
  pSETONEXTintern (g, pSYM(eOnext), pSYM (eOPPOSITE));
  pSETONEXTintern (g, pROT3(qe), pROT3 (eOPPOSITE));
  pSETONEXTintern (g, qe, eOnext);
  pSETONEXTintern (g, pROT3 (eOPPOSITE), pROT1(eOnext));
  pSETONEXTintern (g, eOPPOSITE, pSYM (qe));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* EXPORTED FUNCTIONS */
/* all exported functions which modify the quadedge datastructure */
/* increase the time by 1 unit. i.e also if an external function inserts */
/* several edges the time is only increased by 1 unit. */

/*---------------------------------------------------------------------------*/

void
pqeNew (g, n)

     graphType *g;
     indexType n;

{
  /* n*9 is an heuristic which works ok */
  g->pe = (persistQeType *) calloc ((unsigned int) n*9, sizeof (persistQeType));

  if (g->pe == NULL) {
    (void) printf ("ERROR: pqeNew: calloc failed.\n");
    exit (1);
  }

  g->maxe = (float) n * (float) (9.0 / 4.0);

  NE(g) = 0;
}

/*---------------------------------------------------------------------------*/

indexType
pQNBR (qe) 
     indexType qe;

/* QNBR takes quadedge index and returns 0..3, which of 4 parts qe refers to */

{
  return qe & 3;
}
/*---------------------------------------------------------------------------*/

indexType
pMAKEQE (e)
     indexType e;

/* pMAKEQE takes an edge index and returns quadedge index */

{
  return e << 2;
}

/*---------------------------------------------------------------------------*/

indexType
pQETOE (qe)

     indexType qe;

/* QETOE takes a quadedge index and returns the edge number */

{
  return qe >> 2;
}

/*---------------------------------------------------------------------------*/

indexType
pQETOQE0 (qe)

     indexType qe;

/* QETOQE0 takes a quadedge index and returns qe index with no rotation */

{
  return (qe | 3) ^ 3;
}

/*---------------------------------------------------------------------------*/

indexType
pSYM (qe)

     indexType qe;

{
  return qe ^ 2;
}

/*---------------------------------------------------------------------------*/

indexType
pROT1 (qe)

     indexType qe;

{
  return QETOQE0 (qe) + MOD4 (QNBR (qe) + 1);
}

/*---------------------------------------------------------------------------*/

indexType
pROT3 (qe)

     indexType qe;

{
  return QETOQE0 (qe) + MOD4 (QNBR (qe) + 3);
}

/*---------------------------------------------------------------------------*/

indexType
pORG (g, qe)

     graphType *g;
     indexType qe;

{
  return g->pe[qe].dat;
}

/*---------------------------------------------------------------------------*/

indexType
pDST (g, qe)

     graphType *g;
     indexType qe;

{
  return pORG (g, pSYM (qe));
}

/*---------------------------------------------------------------------------*/

void
pSETORG (g, qe, org)

     graphType *g;
     indexType qe;
     int org;

{
  g->pe[qe].dat = org;
}

/*---------------------------------------------------------------------------*/

void
pSETDST (g, qe, org)

     graphType *g;
     indexType qe;
     int org;

{
  pSETORG (g, pSYM (qe), org);
}

/*---------------------------------------------------------------------------*/

int
pqeISLASTENTRY (nxt, index, maxEntries, additional)

     nextRecordType *nxt;
     indexType index;
     indexType maxEntries;
     additionalNextRecordsType *additional;

/* returns true iff there is no further entry after nxt[index] */

{
  void (*x) () = NULL;
  if ((nxt[index].time != MARKED_LAST) && 
      (nxt[index].edge == MARKED_DELETED)) {
    printf("ERROR: pqeISLASTENTRY: not defined for deleted edge.\n");
    (*x)();
    exit (1);
  }
  
  return (((index < maxEntries - 1) && (nxt[index + 1].time == MARKED_LAST)) ||
	  ((index == maxEntries - 1) && (additional == NULL)));
}

/*---------------------------------------------------------------------------*/

int
pqeISDEADslow (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

{
  nextRecordType *nxt;
  indexType index;
  indexType maxEntries;
  additionalNextRecordsType *additional;

  pqeGetPresent (g, qe, time, &nxt, &index, &maxEntries, &additional);
  return pqeISDEAD (nxt, index, maxEntries, additional);
}

/*---------------------------------------------------------------------------*/

int
pqeISDEAD (nxt, index, maxEntries, additional)

     nextRecordType *nxt;
     indexType index;
     indexType maxEntries;
     additionalNextRecordsType *additional;

/* returns true iff edge does not exist anymore at the time */
/* nxt[index].time */

{
  return ((nxt[index].time != MARKED_LAST) &&
	  (nxt[index].edge == MARKED_DELETED));
}

/*---------------------------------------------------------------------------*/

int
pqeISNEW (g, qe, time)
     
     graphType *g;
     indexType qe;
     indexType time;

/* returns true iff qe came alife at time 'time' */

{
  return (g->pe[qe].nxt[0].time == time);
}

/*---------------------------------------------------------------------------*/

void
pqeGetPresent (g, qe, time, nxt, index, maxEntries, additional)

     graphType *g;
     indexType qe;
     indexType time;
     nextRecordType **nxt;
     indexType *index;
     indexType *maxEntries;
     additionalNextRecordsType ***additional;

/* computes all the information necessary to access the next pointer */
/* at time 'time' and the future thereof in constant time. if time == */
/* -1 then the current time is assumed. */

{
  indexType i;
  additionalNextRecordsType * tmp;

  if (time == TIME_NOW)
    time = g->time;

  /* a little hack for a little bit more speed */

  if (((NOF_NEXT == 1) && (g->pe[qe].additional == NULL)) ||
      ((NOF_NEXT > 1) && 
       ((g->pe[qe].nxt[1].time == MARKED_LAST) ||
	(g->pe[qe].nxt[1].time > time))) ||
      (g->pe[qe].nxt[0].time == MARKED_LAST)) {
    
    *nxt = g->pe[qe].nxt;
    *index = 0;
    *maxEntries = NOF_NEXT;
    *additional = &(g->pe[qe].additional);
    return;

  } else {

    /* the hope for this hack is that g->pe[qe].additional is NULL */
    i = 1;
    while ((i < NOF_NEXT) && (g->pe[qe].nxt[i].time <= time) &&
	   (g->pe[qe].nxt[i].time != MARKED_LAST)) {
      i++;
    };

    if (i < NOF_NEXT) {
      *nxt = g->pe[qe].nxt;
      *index = i - 1;
      *maxEntries = NOF_NEXT;
      *additional = &(g->pe[qe].additional);
      return;
    } else 
      if ((g->pe[qe].additional == NULL) || 
	  (g->pe[qe].additional->nxt[0].time > time)) {
	*nxt = g->pe[qe].nxt;
	*index = NOF_NEXT - 1;
	*maxEntries = NOF_NEXT;
	*additional = &(g->pe[qe].additional);
	return;
      } else { /* there goes the hope */

/*	printPersistQe (&(g->pe[qe]), qe);*/

	tmp = g->pe[qe].additional;
	while ((tmp->additional != NULL) &&
	       (tmp->additional->nxt[0].time <= time)) {
	  tmp = tmp->additional;
	}
	
	i = 1;
	while ((i < NOF_NEXT_ADDITIONAL) && (tmp->nxt[i].time <= time) &&
	       (tmp->nxt[i].time != MARKED_LAST)) {
	  i++;
	};
	
	*nxt = tmp->nxt;
	*index = i - 1;
	*maxEntries = NOF_NEXT_ADDITIONAL;
	*additional = &(tmp->additional);
	return;
      }
  }
}
  
/*---------------------------------------------------------------------------*/

static int
pqeHasFuture (nxt, index, maxEntries, additional)

      nextRecordType *nxt;
     indexType index;
     indexType maxEntries;
     additionalNextRecordsType **additional;

{
  if (index == maxEntries - 1) 
    if ((*additional != NULL) && (nxt[index].edge != MARKED_DELETED))
      return 1;
    else
      return 0;
  else
    if ((nxt[index + 1].time != MARKED_LAST) && 
	(nxt[index].edge != MARKED_DELETED))
      return 1;
    else
      return 0;
}

/*---------------------------------------------------------------------------*/

void
pqeGetFuture (nxt, index, maxEntries, additional,
	      futureNxt, futureIndex, futureMaxEntries, futureAdditional)

     nextRecordType *nxt, **futureNxt;
     indexType index, *futureIndex;
     indexType maxEntries, *futureMaxEntries;
     additionalNextRecordsType **additional, ***futureAdditional;

/* computes the future from the information provided by function */
/* pqeGetPresent. Future is defined as the next change in the nxt */
/* field of an quadedge. this is not necessarily the time immediately */
/* following the present */

{
  if (index == maxEntries - 1)
    if ((*additional != NULL) && (nxt[index].edge != MARKED_DELETED)) {
      *futureNxt = (*additional)->nxt;
      *futureIndex = 0;
      *futureMaxEntries = NOF_NEXT_ADDITIONAL;
      *futureAdditional = &((*additional)->additional);
    } else {
      printf ("ERROR: pqeGetFuture 1: edge %d has no future.\n", nxt);
      exit (1);
    }
  else 
    if ((nxt[index + 1].time != MARKED_LAST) && 
	(nxt[index].edge != MARKED_DELETED)) {
      *futureNxt = nxt;
      *futureIndex = index + 1;
      *futureMaxEntries = maxEntries;
      *futureAdditional = additional;
    } else {
      printf ("ERROR: pqeGetFuture 2: edge %d has no future.\n", nxt);
      exit (1);
    }
}
    
/*---------------------------------------------------------------------------*/

indexType
pONEXT (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

{
  nextRecordType *nxt;
  indexType index;
  indexType maxEntries;
  additionalNextRecordsType **additional;

  pqeGetPresent (g, qe, time, &nxt, &index, &maxEntries, &additional);

  return nxt[index].edge;
}

/*---------------------------------------------------------------------------*/

indexType
pOPREV (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

{
  return pROT1 (pONEXT (g, pROT1 (qe), time));
}

/*---------------------------------------------------------------------------*/

indexType
pLNEXT (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

/* LNEXT returns next quadedge adjoining left facet of current edge */

{
  return pOPREV (g, pSYM (qe), time);
}

/*---------------------------------------------------------------------------*/

indexType
pRNEXT (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

/* RNEXT returns next quadedge adjoining right facet of current edge */

{
  return pONEXT (g, pSYM (qe), time);
}

/*---------------------------------------------------------------------------*/

indexType
pLEFTFACET (g, qe)

     graphType *g;
     indexType qe;

/* given a quad edge, return the number of the facet to its left */

{
  return pORG (g, pROT1 (qe));
}

/*---------------------------------------------------------------------------*/

void
pSETLEFTFACET (g, qe, facet)

     graphType *g;
     indexType qe;
     int facet;

/* given a quad edge, return the number of the facet to its left */

{
  pSETORG (g, pROT1 (qe), facet);
}

/*---------------------------------------------------------------------------*/

void
pSETONEXT (g, qe, next)

     graphType *g;
     indexType qe, next;

{
  (g->time)++;
  pSETONEXTintern (g, qe, next);
}

/*---------------------------------------------------------------------------*/

indexType 
pqeAddSiteSite (g, s1, s2)

     graphType *g;
     indexType s1, s2;	/* the two sites where the edge will be added */
     
{
  (g->time)++;
  return pqeAddSiteSiteIntern (g, s1, s2);
}
  
/*---------------------------------------------------------------------------*/

indexType 
pqeAddEdgeSite (g, a, s)

     graphType *g;
     indexType a;
     indexType s;

{
  (g->time)++;
  return pqeAddEdgeSiteIntern (g, a, s);
}

/*---------------------------------------------------------------------------*/

indexType 
pqeAddEdgeEdge (g, a, b)

     graphType *g;
     indexType a, b;

{
  (g->time)++;
  return pqeAddEdgeEdgeIntern (g, a, b);
}

/*---------------------------------------------------------------------------*/

indexType
pqeFlip (g, qe)

     graphType *g;
     indexType qe;

{
  (g->time)++;
  return pqeFlipIntern (g, qe);
}

/*---------------------------------------------------------------------------*/

void 
pqeDelete (g, qe)

     graphType *g;
     indexType qe;

{
  (g->time)++;
  pqeDeleteIntern (g, qe);
}

/*---------------------------------------------------------------------------*/

indexType
pqeAddTriangleStar (g, qe, p)

     graphType *g;
     indexType qe;
     indexType p;

{
  (g->time)++;
  return pqeAddTriangleStarIntern (g, qe, p);
}

/*---------------------------------------------------------------------------*/

void
pqeDeleteTriangleStar (g, qe)

     graphType *g;
     indexType qe;

{
  (g->time)++;
  pqeDeleteTriangleStarIntern (g, qe);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int
pISCHQE (g, qe, time)

     graphType *g;
     indexType qe;
     indexType time;

/* ISCHQE returns 1 iff qe is convex hull edge */

{
  return 
    (pORG (g, qe) >= 0) && (pDST (g, qe) >= 0) &&
    ((pDST (g, pONEXT (g, qe, time)) < 0) ||
    (pDST (g, pOPREV (g, qe, time)) < 0));
}

/*---------------------------------------------------------------------------*/

int
pISCHedge (g, edge, time)

     graphType *g;
     indexType edge;
     indexType time;

/* ISCHedge returns 1 iff edge is convex hull edge */

{
  return pISCHQE (g, pMAKEQE (edge), time);
}

/*---------------------------------------------------------------------------*/

int
pqeExistsAtTime (g, pqe, time)

     graphType *g;
     indexType pqe;
     indexType time;

{
  indexType t;

  nextRecordType *nxt, *futureNxt;
  indexType index, futureIndex;
  indexType maxEntries, futureMaxEntries;
  additionalNextRecordsType **additional, **futureAdditional;

  t = g->pe[pqe].nxt[0].time;
  
  if (t > time)
    return 0;
  if (t == time)
    return 1;

  pqeGetPresent (g, pqe, time, &nxt, &index, &maxEntries, &additional);
  return ! pqeISDEAD (nxt, index, maxEntries, additional);  
}

/*---------------------------------------------------------------------------*/

static indexType
findPrevPqe (g, pqe, time)
     
     graphType * g;
     indexType pqe;
     indexType time;

{
  indexType tmp;

/*  printPersistQe (&(g->pe[pqe]), pqe);*/

  tmp = pOPREV (g, pqe, time);

  while (tmp != pqe) {
    if (tmp < pqe)
      if ((pORG (g, tmp) >= 0) && (pDST (g, tmp) >= 0))
	return tmp;
    tmp = pOPREV (g, tmp, time);
  }

  return -1;
}

/*---------------------------------------------------------------------------*/

static indexType
findCorrespondingQe (org, dst, g, edgeP)

     indexType org, dst;
     graphType *g;
     indexType *edgeP;

{
  indexType qe;

  qe = edgeP [org];
  if (DST (g, qe) == dst) {
    return qe;
  }

  for (qe = ONEXT (g, qe); qe != edgeP [org]; qe = ONEXT (g, qe))
    if (DST (g, qe) == dst) {
      return qe;
    }
  
  printf ("ERROR: findCorrespondingQe: (%d, %d) has no corresponding qe.\n",
	  org, dst);
  exit (1);
}
  
/*---------------------------------------------------------------------------*/

static void
hDagCopyPQEtoQE (g, pqe, gQe, edgeP, time, nofCH)

     graphType * g;
     indexType pqe;
     graphType * gQe;
     indexType * edgeP;
     indexType time;
     indexType *nofCH;

{
  indexType qeOrg, qeDst, pqeOrg, pqeDst;


/*  printf ("%d %d %d\n", pORG (g, pqe), pDST (g, pqe), pqe);
  printf ("%d %d %d -\n", pDST (g, pqe), pORG (g, pqe), pqe);
*/
  pqeOrg = findPrevPqe (g, pqe, time);
  pqeDst = findPrevPqe (g, pSYM(pqe), time);

  if (pqeOrg >= 0) {
    qeOrg = findCorrespondingQe (pORG(g, pqeOrg), pDST (g, pqeOrg), gQe, edgeP);
    if (pqeDst >= 0) {
      qeDst = findCorrespondingQe (pORG(g, pqeDst), pDST (g, pqeDst),
				   gQe, edgeP);
      qeAddEdgeEdge (gQe, pSYM (qeOrg), qeDst);
    } else {
      qeAddEdgeSite (gQe, pSYM (qeOrg), pDST (g, pqe));
    }
  } else {
    if (pqeDst >= 0) {
      qeDst = findCorrespondingQe (pORG(g, pqeDst), pDST (g, pqeDst),
				   gQe, edgeP);
      qeAddEdgeSite (gQe, pSYM (qeDst), pORG (g, pqe));
    } else {
      qeAddSiteSite (gQe, pORG (g, pqe), pDST (g, pqe));
    }
  }

  edgeP [ORG (gQe, MAKEQE (NE (gQe) - 1))] = MAKEQE (NE (gQe) - 1);
  edgeP [DST (gQe, MAKEQE (NE (gQe) - 1))] = SYM (MAKEQE (NE (gQe) - 1));

  if (pISCHQE (g, pqe, time)) {
    SETCHedge (gQe, NE (gQe) - 1);
    (*nofCH)++;
  } else
    UNSETCHedge (gQe, NE (gQe) - 1);
}

/*---------------------------------------------------------------------------*/

static indexType *
initialEdgeP (g)

     graphType *g;

{
  indexType * tmp;
  indexType vertex;

  tmp = (indexType *) calloc((unsigned int) NS(g), 
			     sizeof(indexType));
  if (tmp == NULL) {
    printf("initialEdgeP: fail on table allocation.\n");
    exit (1);
  }

  for (vertex = 0; vertex < NS(g); vertex++) 
    tmp [vertex] = -1;

  return tmp;
}

/*---------------------------------------------------------------------------*/

void
copyHdagToQe (g, time, nofQe, newQe, nofCH)

     graphType *g;
     indexType time;
     indexType *nofQe;
     qe1Type **newQe;
     indexType *nofCH;

/* copies the version of the triangulation in the hdag as it was at */
/* time 'time' to a quadedge datastructure */

{
  indexType pqe, qe;
  graphType gTmp, *gTmpP;
  indexType *edgeP; /* pointers from vertices to one edge incident to */
		    /* the vertex */
  double dummy;

  if (time == -1)
    time = g->time;

  gTmpP = &gTmp;

  /* allocate memory for the table pqeToQe. this table is the function */
  /* from the persistent quadedges to the quadedges. it is needed in */
  /* order to copy in O(n) */

  edgeP = initialEdgeP (g);

  /* allocate memory for the new quadedge data structure */
  gTmpP->e = 
    (qe1Type *) calloc((unsigned int) NS(g) * 3 - 6, 4 * sizeof(qe1Type));
  if (gTmpP->e == NULL) {
    printf("copyHdagToQe: fail on quadedge allocation.\n");
    exit (1);
  }

  NE(gTmpP) = 0;
  gTmpP->maxe = NS(g) * 3 - 6;
  *nofCH = 0;

  for (pqe = 0; pqe < NE (g) * 4; pqe += 4) {

    if ((pORG (g, pqe) >= 0) && (pDST (g, pqe) >= 0)) /* only finite */
						      /* vertices */
						      /* incident ? */
      if (pqeExistsAtTime (g, pqe, time))  /* does it exist at 'time' ? */
	hDagCopyPQEtoQE (g, pqe, gTmpP, edgeP, time, nofCH);
  }

  /* reallocate memory for the new quadedge data structure */
  gTmpP->e = (qe1Type *) 
    realloc((char *) gTmpP->e,
	    (unsigned int) NE(gTmpP) * 4 * sizeof(qe1Type));
  if (gTmpP->e == NULL) {
    printf("copyHdagToQe: fail on quadedge allocation.\n");
    exit (1);
  }

  gTmpP->s = g->s;

  for (qe = 0; ! ISCHedge (gTmpP, QETOE (qe)); qe += 4) {
  }

  if (ISCHedge (gTmpP, QETOE (OPREV (gTmpP, qe))))
    if (leftTurn (gTmpP, DST (gTmpP, OPREV (gTmpP, qe)),
		  ORG (gTmpP, qe), DST (gTmpP, qe), &dummy))
      gTmpP->chEdge = qe;
    else
      gTmpP->chEdge = SYM (qe);
  else
    if (leftTurn (gTmpP, DST (gTmpP, qe), ORG (gTmpP, qe), 
		  DST (gTmpP, ONEXT (gTmpP, qe)), &dummy))
      gTmpP->chEdge = SYM (qe);
    else
      gTmpP->chEdge = qe;

  printf("\n");

  gTmpP->maxe = NE(g);
  *nofQe = NE(gTmpP);
  *newQe = gTmpP->e;

  (void) free ((char *) edgeP);
}
  
/*---------------------------------------------------------------------------*/

void 
printPqeStats (g)

     graphType *g;

{
 int count[26];
 int i, c;

 for (i = 0; i < 26; i++)
   count[i] = 0;

 for (i = 0; i < NE (g) * 4; i++)
   if ((c = countNofNext (&(g->pe[i]))) <= 25)
     count [c] += 1;
   else
     count [0] += 1;

 printf ("Next Pointer Length Distribution:\n\n");

 for (i = 1; i <= 13; i++)
   printf ("%6d", i);
 printf ("\n");
 for (i = 1; i <= 13; i++)
   printf ("%6d", count[i]);
 printf ("\n\n");

 for (i = 14; i <= 25; i++)
   printf ("%6d", i);
   printf ("     >\n");
 for (i = 14; i <= 25; i++)
   printf ("%6d", count[i]);
   printf ("%6d", count[0]);
 printf ("\n");
 printf ("\n");
}
/*---------------------------------------------------------------------------*/

void
disposePqe (g)

     graphType *g;

{
  int i;
  additionalNextRecordsType * additional, * tmp;

  for (i = 0; i < NE(g); i++) {
    additional = g->pe[i].additional;
    while (additional != NULL) {
      tmp = additional->additional;
      free ((void *) additional);
      additional = tmp;
    }
  }

  free ((void *) g->pe);
  g->pe = NULL;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
