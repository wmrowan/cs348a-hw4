/*
 * NAME: persistent.quadedge.c
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ERRFILE stdout

int qeDebug = 0;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define MOD4(n) ((n) & 3)

/*---------------------------------------------------------------------------*/

indexType
SYM(qe)

     indexType qe;

{
  return qe ^ 2;
}

/*---------------------------------------------------------------------------*/

indexType
ORG(g, qe)

     graphType *g;
     indexType qe;

{
  return g->e[qe].dat;
}

/*---------------------------------------------------------------------------*/

indexType
DST(g, qe)

     graphType *g;
     indexType qe;

{
  return ORG(g, SYM(qe));
}

/*---------------------------------------------------------------------------*/

void
SETORG(g, qe, org)

     graphType *g;
     indexType qe;
     int org;

{
  g->e[qe].dat = org;
}

/*---------------------------------------------------------------------------*/

void
SETDST(g, qe, org)

     graphType *g;
     indexType qe;
     int org;

{
  SETORG(g, SYM(qe), org);
}

/*---------------------------------------------------------------------------*/

indexType
ROT1 (qe)

     indexType qe;

{
  return QETOQE0(qe) + MOD4(QNBR(qe) + 1);
}

/*---------------------------------------------------------------------------*/

indexType
ROT3 (qe)

     indexType qe;

{
  return QETOQE0(qe) + MOD4(QNBR(qe) + 3);
}

/*---------------------------------------------------------------------------*/

indexType
ONEXT (g, qe)

     graphType *g;
     indexType qe;

{
  return g->e[qe].nxt;
}

/*---------------------------------------------------------------------------*/

indexType
OPREV (g, qe)

     graphType *g;
     indexType qe;

{
  return ROT1(ONEXT(g, ROT1(qe)));
}

/*---------------------------------------------------------------------------*/

void
SETONEXT (g, qe, next)

     graphType *g;
     indexType qe, next;

{
  g->e[qe].nxt = next;
}

/*---------------------------------------------------------------------------*/

indexType
LNEXT (g, qe)

     graphType *g;
     indexType qe;

/* LNEXT returns next quadedge adjoining left facet of current edge */

{
  return OPREV(g, SYM(qe));
}

/*---------------------------------------------------------------------------*/

indexType
RNEXT (g, qe)

     graphType *g;
     indexType qe;

/* RNEXT returns next quadedge adjoining right facet of current edge */

{
  return ONEXT(g, SYM(qe));
}

/*---------------------------------------------------------------------------*/

indexType
EMINA (g, n)

     graphType *g;
     int n;

/* access macros for min and max alpha indices for an edge */

{
  return ORG (g, MAKEQE(n) + 1);
}

/*---------------------------------------------------------------------------*/

indexType
EMAXA (g, n)

     graphType *g;
     indexType n;

/* access macros for min and max alpha indices for an edge */

{
  return ORG (g, MAKEQE(n) + 3);
}

/*---------------------------------------------------------------------------*/

indexType
LEFTFACET (g, qe)

     graphType *g;
     indexType qe;

/* given a quad edge, return the number of the facet to its left */

{
  return ORG(g, ROT1(qe));
}


/* given a quad edge, return the number of the facet to its left */
#define LEFTFACET(g, qe) ORG(g, ROT1(qe))

/*---------------------------------------------------------------------------*/

void
SETEMINA (g, n, pt)

     graphType *g;
     int n;
     int pt;

/* access macros for min and max alpha indices for an edge */

{
  SETORG (g, MAKEQE(n) + 1, pt);
}

/*---------------------------------------------------------------------------*/

void
SETEMAXA (g, n, pt)

     graphType *g;
     indexType n;
     int pt;

/* access macros for min and max alpha indices for an edge */

{
  SETORG (g, MAKEQE(n) + 3, pt);
}

/*---------------------------------------------------------------------------*/

void
SETLEFTFACET (g, qe, facet)

     graphType *g;
     indexType qe;
     int facet;

/* given a quad edge, return the number of the facet to its left */

{
  SETORG(g, ROT1(qe), facet);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

indexType qeAddSiteSite(g, s1, s2)
graphType *g;
indexType s1, s2;	/* the two sites where the edge will be added */
/*
 *   qeAddSiteSite adds an edge from site s1 to site s2, assuming that
 * neither site already has an edge connected to it.
 */
{
  indexType qenew;

  if (NE(g) == g->maxe) {
    (void) fprintf(ERRFILE, "qeAddSiteSite: edge capacity exceeded.\n");
    return(-1);
  }

  if (qeDebug) 
    (void) printf(
      "qeAddSiteSite: adding edge %d from %d to %d\n", NE(g), s1, s2);
  
  qenew = MAKEQE(NE(g));
  NE(g) += 1;

  SETORG(g, qenew, s1);
  SETORG(g, SYM(qenew), s2);

  SETONEXT(g, qenew, qenew);
  SETONEXT(g, qenew +1, ROT3(qenew));
  SETONEXT(g, qenew +2, SYM(qenew));
  SETONEXT(g, qenew +3, ROT1(qenew));
  return(qenew);
}  /* -------------------- qeAddSiteSite -------------------- */

/*---------------------------------------------------------------------------*/

indexType qeAddEdgeSite(g, a, s)

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

  aRnext = ONEXT(g, SYM(a));

  if (NE(g) == g->maxe) {
    (void) fprintf(ERRFILE, "qeAddEdgeSite: edge capacity exceeded.\n");
    return(-1);
  }

  if (qeDebug) 
    (void) printf("qeAddEdgeSite: adding edge %d from site %d to %d\n", 
      NE(g), DST(g, a), s);

  qenew = MAKEQE(NE(g));
  NE(g) += 1;

  SETORG(g, qenew, DST(g, a));
  SETORG(g, SYM(qenew), s);

  SETONEXT(g, qenew, aRnext);
  SETONEXT(g, qenew +1, ROT1(a));
  SETONEXT(g, qenew +2, SYM(qenew));
  SETONEXT(g, qenew +3, ROT1(qenew));

  SETONEXT(g, SYM(a), qenew);
  SETONEXT(g, ROT1(aRnext), ROT3(qenew));
  return(qenew);
}  /* -------------------- qeAddEdgeSite -------------------- */

/*---------------------------------------------------------------------------*/

indexType qeAddEdgeEdge(g, a, b)
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

  aRnext = ONEXT(g, SYM(a));
  bLprev = SYM(ONEXT(g, b));

  if (NE(g) == g->maxe) {
    (void) fprintf(ERRFILE, "qeAddEdgeEdge: edge capacity exceeded.\n");
    return(-1);
  }

  if (qeDebug) 
    (void) printf("qeAddEdgeEdge: adding edge %d from site %d to %d\n", 
      NE(g), DST(g, a), ORG(g, b));

  qenew = MAKEQE(NE(g));
  NE(g) += 1;

  SETORG(g, qenew, DST(g, a));
  SETORG(g, SYM(qenew), ORG(g, b));

  SETONEXT(g, qenew, ONEXT(g, SYM(a)));
  SETONEXT(g, qenew +1, ROT1(a));
  SETONEXT(g, qenew +2, ONEXT(g, b));
  SETONEXT(g, qenew +3, ROT3(b));

  SETONEXT(g, SYM(a), qenew);
  SETONEXT(g, b, SYM(qenew));
  SETONEXT(g, ROT1(aRnext), ROT3(qenew));
  SETONEXT(g, ROT3(bLprev), ROT1(qenew));
  return(qenew);
}  /* -------------------- qeAddEdgeEdge -------------------- */

/*---------------------------------------------------------------------------*/

static char *ename(qe)
indexType qe;
{
  static char *mod[] = { "", "Rot", "Sym", "Rot3" };
  static char *undef = { "undefined" };
  static char answer[8];

  if (qe < 0) return(undef);

  (void) sprintf(answer, "e%d %s", QETOE(qe), mod[QNBR(qe)]);
  return(answer);
}  /* -------------------- ename -------------------- */

/*---------------------------------------------------------------------------*/

void qeFlip(g, qe)
graphType *g;
indexType qe;
/*            *                         *
 * eSymOprev /^\ eSymOnext             / \
 *          / | \                     / e \
 *         * e|  *          ===>     *<----*
 *   eOnext \ | / eOprev              \   /
 *           \|/                       \ /
 *            *                         *
 */
{
  indexType eOnext, eOprev, eSymOnext, eSymOprev;

  if (qeDebug) (void) printf("qeFlip: flipping edge %s\n", ename(qe));

  /* save neighboring edges in original configuration */
  eOnext = ONEXT(g, qe);
  eOprev = OPREV(g, qe);
  eSymOnext = ONEXT(g, SYM(qe));
  eSymOprev = OPREV(g, SYM(qe));
  
  /* set sites to new values */
  SETORG(g, qe, DST(g, ONEXT(g, SYM(qe))));	/* new e Org */
  SETORG(g, SYM(qe), DST(g, ONEXT(g, qe)));	/* new e Dest */

  /* set nxt pointers of the flipped edge */
  SETONEXT(g, qe, SYM(eOprev));
  SETONEXT(g, ROT1(qe), ROT1(eSymOnext));
  SETONEXT(g, SYM(qe), SYM(eSymOprev));
  SETONEXT(g, ROT3(qe), ROT1(eOnext));

  /* Update pointers of neighboring edges. */
  SETONEXT(g, ROT1(eOnext), ROT3(eOprev));
  SETONEXT(g, SYM(eOnext), SYM(qe));

  SETONEXT(g, eOprev, eOnext);
  SETONEXT(g, ROT3(eOprev), ROT3(qe));

  SETONEXT(g, ROT1(eSymOnext), ROT3(eSymOprev));
  SETONEXT(g, SYM(eSymOnext), qe);

  SETONEXT(g, eSymOprev, eSymOnext);
  SETONEXT(g, ROT3(eSymOprev), ROT1(qe));


}  /* -------------------- qeFlip -------------------- */

/*---------------------------------------------------------------------------*/

void qeDelete(g, qe)

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

/* CAUTION: the fields for qe are not updated, and qe is not marked as deleted
            and non of the other functions
            knows about the deleted edge! so the application must take care
	    of this! use qeReinsert to use qe again (not necessarily in the
	    original position). */

{
  indexType eOnext, eOprev, eSymOnext, eSymOprev;

  if (qeDebug) (void) printf("qeFlip: flipping edge %s\n", ename(qe));

  /* save neighboring edges in original configuration */
  eOnext = ONEXT(g, qe);
  eOprev = OPREV(g, qe);
  eSymOnext = ONEXT(g, SYM(qe));
  eSymOprev = OPREV(g, SYM(qe));

  /* Update pointers of neighboring edges. */
  SETONEXT(g, ROT1(eOnext), ROT3(eOprev));
  SETONEXT(g, eOprev, eOnext);
  SETONEXT(g, ROT1(eSymOnext), ROT3(eSymOprev));
  SETONEXT(g, eSymOprev, eSymOnext);

}

/*---------------------------------------------------------------------------*/

void
qeReinsert (g, newQe, newA, newB)

     graphType *g;
     indexType newQe, newA, newB;

/*
 *   qeReinsert adds an edge to between the site pointed to by edge
 * a and the site which is the origin of edge b.  The following equalities
 * must to hold: aRight == qenewRight and qenewLeft == bLeft.
 *
 *    \ aRnext       ^
 *     \    qenew    | b
 *      *- - - - - ->*
 *      ^             \
 *      |a             \ bLprev
 */

/* this function is similar to the function qeAddEdgeEdge. The only difference
   is that the qe which should be used as qenew can be specified. this is
   useful if qe has been deleted with function qeDelete, e.g.. */

{
  indexType aRnext, bLprev;
  indexType qe, a, b;

  if (MAKEQE (QETOE (newQe)) == newQe) {
    qe = newQe;
    a = newA;
    b = newB;
  } else {
    qe = SYM (newQe);
    a = SYM (newB);
    b = SYM (newA);
  }

  if (MAKEQE (QETOE (qe)) != qe)
    printf ("qeReinsert Error %d %d \n", MAKEQE (QETOE (qe)), qe);
  
  aRnext = ONEXT(g, SYM(a));
  bLprev = SYM(ONEXT(g, b));

  if (qeDebug) 
    (void) printf("qeReinsert: adding edge %d from site %d to %d\n", 
      NE(g), DST(g, a), ORG(g, b));

  SETORG(g, qe, DST(g, a));
  SETORG(g, SYM(qe), ORG(g, b));

  SETONEXT(g, qe, ONEXT(g, SYM(a)));
  SETONEXT(g, qe +1, ROT1(a));
  SETONEXT(g, qe +2, ONEXT(g, b));
  SETONEXT(g, qe +3, ROT3(b));

  SETONEXT(g, SYM(a), qe);
  SETONEXT(g, b, SYM(qe));
  SETONEXT(g, ROT1(aRnext), ROT3(qe));
  SETONEXT(g, ROT3(bLprev), ROT1(qe));
} 

/*---------------------------------------------------------------------------*/

void
qeDuplicate (gSource, gDest, qe)

     graphType *gSource, *gDest;
     indexType qe;


{
  indexType tmp;
  
  tmp = QETOQE0 (qe);
  
  gDest->e[tmp] = gSource->e[tmp];
  gDest->e[tmp+1] = gSource->e[tmp+1];
  gDest->e[tmp+2] = gSource->e[tmp+2];
  gDest->e[tmp+3] = gSource->e[tmp+3];
}

/*---------------------------------------------------------------------------*/

void qeRenumberEdges(g)
graphType *g;
/*
 *   qeRenumberEdges accepts a convex hull graph and one quadedge
 * which is known to be a counter-clockwise edge on the hull.  The
 * edges are renumbered so that the hull edges are numbered 0..nChEdges-1
 * and the interior edges are numbered nChEdges..NE(g)-1.
 */
{
  indexType *map;
  register indexType i;
  indexType j, eNumber, newe, chqe, quadPart, onext, oldQeBase, newQeBase;

  qe1Type *qetbl;

  /* allocate space for array to map old edges to new, and initialize */
  map = (indexType *) calloc((unsigned int) NE(g), sizeof(indexType));
  if (map == NULL) {
    (void) fprintf(stdout, "qeRenumberEdges: calloc failed.\n");
    exit(1);
  }

  for (i=0; i< NE(g); i++) map[i] = -1;

  /* build up map for convex hull edges */
  newe = 0;
  chqe = g->chEdge;			/* ccw convex hull edge on old graph */
  do {
    eNumber = QETOE(chqe);
    map[eNumber] = newe;
    newe++;
    chqe = ONEXT(g, SYM(chqe));	/* next ccw ch edge on old graph */
  } while (chqe != g->chEdge);

  g->nChEdges = newe;			/* save number of convex hull edges */

  /* define mapping for non convex hull edges */
  for (i=0; i< NE(g); i++)
    if (map[i] < 0) {			/* if not defined yet */
      map[i] = newe;
      newe++;
    }

  /* allocate space for temporary edge table */
  qetbl = (qe1Type *) calloc((unsigned int) NE(g), 4 * sizeof(qe1Type));
  if (qetbl == NULL)
    (void) fprintf(ERRFILE, "qeRenumberEdges: fail on calloc.\n");

  /* map data for each edge into the qetbl temporary storage */
  for (i=0; i< NE(g); i++) {
    oldQeBase = MAKEQE(i);
    newQeBase = MAKEQE(map[i]);
    for (j=0; j<4; j++) {
      qetbl[newQeBase + j].nxt = ONEXT(g, oldQeBase + j);
      qetbl[newQeBase + j].dat = ORG(g, oldQeBase + j);
    }
  }

  /* write back into original quadedge array, mapping individual onext parts */
  for (i=0; i< MAKEQE(NE(g)); i++) {
    onext = qetbl[i].nxt;
    quadPart = QNBR(onext);
    SETONEXT(g, i, MAKEQE(map[QETOE(onext)]) + quadPart);
    SETORG(g, i, qetbl[i].dat);
  }

  free((char *) qetbl);
  free((char *) map);

  /* renumbering edges means chEdge must be modified too */
  g->chEdge = 0;

  (void) printf("qeRenumberEdges:  edge renumbering completed.\n");
}  /* -------------------- qeRenumberEdges -------------------- */

/*---------------------------------------------------------------------------*/

void qeChCopy(gSrc, gDst)
graphType *gSrc, *gDst;
/*
 *   This routine copies the convex hull edges from gSrc to gDst,
 * given a counter-clockwise convex hull quadedge in the source graph.
 * It is assumed that the convex hull edges are numbered
 * 0 .. nChEdges-1, as the routine qeRenumberEdges above would do.
 */
{
  indexType i, prevE, qe, nextE;

  /* initially copy everything, "verbatim" */
  for (i=0; i< MAKEQE(gSrc->nChEdges); i++) {
    SETONEXT(gDst, i, ONEXT(gSrc, i));
    SETORG(gDst, i, ORG(gSrc, i));
  }

  /* fix parts of the copied quadedges which could refer to interior edges */
  qe = gSrc->chEdge;
  nextE = ONEXT(gSrc, SYM(qe));
  do {
    prevE = qe;
    qe = nextE;
    nextE = ONEXT(gSrc, SYM(qe));

    SETONEXT(gDst, qe, SYM(prevE));
    SETONEXT(gDst, ROT3(qe), ROT3(nextE));
  } while (qe != gSrc->chEdge);

  NE(gDst) = gSrc->nChEdges;
  gDst->chEdge = gSrc->chEdge;
  gDst->nChEdges = gSrc->nChEdges;
  gDst->nDtcEdges = gSrc->nDtcEdges;

  (void) printf("qeChCopy: convex hull edges copied to new graph\n");
}  /* -------------------- qeChCopy -------------------- */

/*---------------------------------------------------------------------------*/

void qeMeld(gDst, gSrc)
graphType *gDst, *gSrc;
/*
 *   qeMeld copies edges from gSrc into gDst, so that gDst
 * ends up as the combination of the two graphs.  This routine is
 * not general purpose, it has several expectations regarding the
 * graphs input to it:
 *
 *   1.  Both graphs are expected to have corresponding perimeter (convex
 *     hull) edges numbered 0..nChEdges-1.
 *   2.  The numbering of interior edges for each of the two graphs should
 *     not conflict.
 *
 *   This is routine is used in the alpha shapes program to join the closest
 * point and furthest point Delaunay triangulations, leaving the result
 * in the closest point structure (gDst).  Both graphs have
 * corresponding convex hull edges, numbered 0..nChEdges-1.  The
 * closest point graph has interior edges numbers nChEdges..nDtcEdges-1,
 * while the interior edges of the furthest point graph are numbered
 * nDtcEdges..nDtcEdges+nDtfInteriorEdges-1.  The joining can be
 * visualized as sewing together the two halves of a ball, where the
 * seam runs along the convex hull edges.  The resulting graph can be
 * interpreted as if the viewer were inside the ball with the closest
 * point graph below and the furthest point graph (viewed from the
 * flip side) above.
 */
{
  indexType e, qe, quadBase, nextQe, j;

  /* copy interior edges of dtf into dt graph */
  for (e= NE(gDst); e< NE(gSrc); e++) {
    quadBase = MAKEQE(e);

    /* copy Onext and SymOnext over, flipping along the way */
    for (j=0; j<4; j += 2) {
      qe = quadBase + j;
      SETONEXT(gDst, qe, ROT1(ONEXT(gSrc, ROT1(qe))));
      SETORG(gDst, qe, ORG(gSrc, qe));
    }

    /* copy RotOnext and Rot3Onext parts over, flipping along the way */
    for (j=1; j<4; j += 2) {
      qe = quadBase + j;
      SETONEXT(gDst, qe, ROT3(ONEXT(gSrc, ROT3(qe))));
      SETORG(gDst, qe, ORG(gSrc, qe));
    }
  }

  /* modify the RotOnext and SymOnext slots in each convex hull edge */
  qe = gSrc->chEdge;
  do {
    /* find the next ch edge before modifying the current one (qe) */
    nextQe = ONEXT(gDst, SYM(qe)); 

    SETONEXT(gDst, ROT1(qe), ROT3(ONEXT(gSrc, qe)));
    SETONEXT(gDst, SYM(qe),  ROT1(ONEXT(gSrc, ROT3(qe))));

    qe = nextQe;
  } while (qe != gSrc->chEdge);

  /* change the edge count in the modified graph */
  NE(gDst) = NE(gSrc);
}  /* -------------------- qeMeld -------------------- */

/*---------------------------------------------------------------------------*/

void qeDump(fp, g)
FILE *fp;
graphType *g;
{
  int i, j;
  indexType qe;

  if (! NE(g)) return;

  for (i=0; i< NE(g); i++) {
    (void) fprintf(fp, "E %d:", i);
    for (j=0; j<=3; j++) {
      qe = MAKEQE(i) + j;
      (void) fprintf(fp, "\t%5d  %s\n", ORG(g, qe), ename(ONEXT(g, qe)));
     }
  }
  (void) fprintf(fp, "\n");
}  /* -------------------- qeDump -------------------- */

/*---------------------------------------------------------------------------*/

int qeVerify(g)
graphType *g;
{
  indexType e, qe[2];
  int i, err;

  if (NE(g) <= 0) {
    (void) printf("qeVerify: no edges\n");
    return(0);
  }

  for (e=0; e < NE(g); e++) {
    qe[0] = MAKEQE(e);
    qe[1] = SYM(qe[0]);

    err = 0;
    for (i=0; i<2; i++) {
      err |=  (ORG(g, qe[i])) == (DST(g, qe[i]));

      err |= (ORG(g, qe[i])) != (ORG(g, ROT1(ONEXT(g, ROT1(qe[i])))));

      err |= (ORG(g, qe[i])) != 
        (ORG(g, ROT1(ONEXT(g, ROT1(ONEXT(g ,qe[i]))))));
    }

    if (err) {
      (void) fprintf(ERRFILE, "qeVerify: bad edge %d\n", e);
      qeDump(ERRFILE, g);
    }
  }
  
  return err;
}  /* ------------------- qeVerify ------------------- */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void qeReserveEdges(g, n)
graphType *g;
int n;
/*
 *   Add n to the edge count of the quadedge structure so that those
 * edges will be reserved for future use.
 */
{
  if (NE(g) + n > g->maxe)
    (void) fprintf(ERRFILE, "qeReserveEdges: edge overflow.\n");

  else
    NE(g) += n;
}  /* -------------------- qeReserveEdges ------------------- */

/*---------------------------------------------------------------------------*//*---------------------------------------------------------------------------*/

void 
qeResetCHstatus (g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    UNSETCHedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/

void 
qeSetCHstatus (g)

     graphType * g;

{
  int chQe;

  qeResetCHstatus (g);

  chQe = g->chEdge;
  do {
    SETCHedge (g, QETOE (chQe));
    chQe = ONEXT(g, SYM(chQe));
  } while (chQe != g->chEdge);
}

/*---------------------------------------------------------------------------*/

void 
qeResetCONSTRstatus (g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    UNSETCONSTRedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/

void 
qeResetFINALstatus (g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    UNSETFINALedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/

void 
qeResetDELETEDstatus (g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    UNSETDELETEDedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/

void 
qeSetDELETEDstatus (g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    SETDELETEDedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/

void 
qeResetALLstati(g)

     graphType * g;

{
  int i;

  for (i = 0; i < NE(g); i++) {
    UNSETCHedge (g, i);
    UNSETCONSTRedge (g, i);
    UNSETFINALedge (g, i);
    UNSETDELETEDedge (g, i);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* here are the function equivalents of the macros in the original 
implementation of the quadedges */

/*---------------------------------------------------------------------------*/

indexType
QNBR(qe) 
     indexType qe;

/* QNBR takes quadedge index and returns 0..3, which of 4 parts qe refers to */

{
  return qe & 3;
}
/*---------------------------------------------------------------------------*/

indexType
MAKEQE(e)
     indexType e;

/* MAKEQE takes an edge index and returns quadedge index */

{
  return e << 2;
}

/*---------------------------------------------------------------------------*/

indexType
QETOE(qe)

     indexType qe;

/* QETOE takes a quadedge index and returns the edge number */

{
  return qe >> 2;
}

/*---------------------------------------------------------------------------*/

indexType
QETOQE0(qe)

     indexType qe;

/* QETOQE0 takes a quadedge index and returns qe index with no rotation */

{
  return (qe | 3) ^ 3;
}

/*---------------------------------------------------------------------------*/

int
ISCHQE (g, qe)

     graphType *g;
     indexType qe;

/* ISCHQE returns 1 iff qe is convex hull edge */

{
  return g->e[qe].status.isCH;
}

/*---------------------------------------------------------------------------*/

void
SETCHQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isCH = 1;
}

/*---------------------------------------------------------------------------*/

void
UNSETCHQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isCH = 0;
}

/*---------------------------------------------------------------------------*/

int
ISCONSTRQE (g, qe)

     graphType *g;
     indexType qe;

/* ISCONSTRQE returns 1 iff qe is a constrained edge */

{
  return g->e[qe].status.isConstrained;
}

/*---------------------------------------------------------------------------*/

void
SETCONSTRQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isConstrained = 1;
}

/*---------------------------------------------------------------------------*/

void
UNSETCONSTRQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isConstrained = 0;
}

/*---------------------------------------------------------------------------*/

int
ISFINALQE (g, qe)

     graphType *g;
     indexType qe;

/* ISFINAL returns 1 iff qe is an edge which should appear in the final
   triangulation, this variable has to be set and reset by the application
   program */

{
  return g->e[qe].status.isFinal;
}

/*---------------------------------------------------------------------------*/

void
SETFINALQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isFinal = 1;
}

/*---------------------------------------------------------------------------*/

void
UNSETFINALQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isFinal = 0;
}

/*---------------------------------------------------------------------------*/

int
ISDELETEDQE (g, qe)

     graphType *g;
     indexType qe;

{
  return g->e[qe].status.isDeleted;
}

/*---------------------------------------------------------------------------*/

void
SETDELETEDQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isDeleted = 1;
}

/*---------------------------------------------------------------------------*/

void
UNSETDELETEDQE(g, qe)

     graphType *g;
     indexType qe;

{
  g->e[qe].status.isDeleted = 0;
}

/*---------------------------------------------------------------------------*/

int
ISCHedge (g, edge)

     graphType *g;
     indexType edge;

/* ISCHedge returns 1 iff edge is convex hull edge */

{
  return ISCHQE (g, MAKEQE (edge));
}

/*---------------------------------------------------------------------------*/

void
SETCHedge (g, edge)

     graphType *g;
     indexType edge;

{
  SETCHQE (g, MAKEQE(edge));
  SETCHQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

void
UNSETCHedge (g, edge)

     graphType *g;
     indexType edge;

{
  UNSETCHQE (g, MAKEQE(edge));
  UNSETCHQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

int
ISCONSTRedge (g, edge)

     graphType *g;
     indexType edge;

/* ISCONSTRedge returns 1 iff edge is a constrained edge */

{
  return ISCONSTRQE (g, MAKEQE (edge));
}

/*---------------------------------------------------------------------------*/

void
SETCONSTRedge (g, edge)

     graphType *g;
     indexType edge;

{
  SETCONSTRQE (g, MAKEQE(edge));
  SETCONSTRQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

void
UNSETCONSTRedge (g, edge)

     graphType *g;
     indexType edge;

{
  UNSETCONSTRQE (g, MAKEQE(edge));
  UNSETCONSTRQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

int
ISFINALedge (g, edge)

     graphType *g;
     indexType edge;

/* ISFINALedge returns 1 iff edge is an edge which should appear in the final
   triangulation, this variable has to be set and reset by the application
   program*/

{
  return ISFINALQE (g, MAKEQE (edge));
}

/*---------------------------------------------------------------------------*/

void
SETFINALedge (g, edge)

     graphType *g;
     indexType edge;

{
  SETFINALQE (g, MAKEQE(edge));
  SETFINALQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

void
UNSETFINALedge (g, edge)

     graphType *g;
     indexType edge;

{
  UNSETFINALQE (g, MAKEQE(edge));
  UNSETFINALQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

int
ISDELETEDedge (g, edge)

     graphType *g;
     indexType edge;

/* ISDELETEDedge returns 1 iff edge is an edge in the current graph,
   this variable has to be set and reset by the application
   program*/

{
  return ISDELETEDQE (g, MAKEQE (edge));
}

/*---------------------------------------------------------------------------*/

void
SETDELETEDedge (g, edge)

     graphType *g;
     indexType edge;

{
  SETDELETEDQE (g, MAKEQE(edge));
  SETDELETEDQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/

void
UNSETDELETEDedge (g, edge)

     graphType *g;
     indexType edge;

{
  UNSETDELETEDQE (g, MAKEQE(edge));
  UNSETDELETEDQE (g, SYM (MAKEQE(edge)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
