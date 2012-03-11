/*
 * NAME: hdag.c
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>
#include "road.h"
#include "quadedge.h"
#include "graph.h"
#include "hdag.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int (*leftTurnTest) ();
int (*terminationCriterion) ();

static indexType
doLocatePoint (graphType*, indexType, indexType, indexType, nextRecordType*, indexType, indexType,
	       additionalNextRecordsType*, int*);
	       
static indexType
doubleTest (graphType*, indexType, indexType, indexType, nextRecordType*, indexType, indexType, 
         additionalNextRecordsType*, int*);
         
static indexType
tripleTest (graphType*, indexType, indexType, indexType, nextRecordType*, indexType, indexType, 
         additionalNextRecordsType*, int*);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

indexType
hdagLocatePoint (g, p, localLeftTurnTest, localTerminationCriterion, spl)

     graphType * g;
     indexType p;
     int (*localLeftTurnTest) ();
     int (*localTerminationCriterion) ();
     int *spl;

{
  nextRecordType *nxt;
  indexType index;
  indexType maxEntries;
  additionalNextRecordsType ** additional;

  leftTurnTest = localLeftTurnTest;
  terminationCriterion = localTerminationCriterion;

  pqeGetPresent (g, g->topTriangleEdge, TIME_0 + 3, &nxt, &index, &maxEntries,
		 &additional);

/*  printf("hdagLocatePoint\n");
  printPersistQe (&(g->pe[ g->topTriangleEdge]),  g->topTriangleEdge);
  printPersistQe2 (nxt, index, maxEntries, *additional);
*/
  return doLocatePoint (g, g->topTriangleEdge, p, TIME_0 + 3, 
			nxt, index, maxEntries, *additional, spl);
}

/*---------------------------------------------------------------------------*/

static indexType
doLocatePoint (g, triEdge, p, time, nxt, index, maxEntries,
	       additional, spl)

     graphType * g;
     indexType triEdge; /* one quad edge of the triangle which containes p */
			/* at time 'time'. the quadedge is */
			/* counterclockwise. */
     indexType p;
     indexType time;
     nextRecordType *nxt; /* pointer to the array which contains the */
			  /* next pointer for time 'time' as its */
			  /* 'index'th entry */
     indexType index;
     indexType maxEntries;  /* max # of entries nxt can contain */
     additionalNextRecordsType * additional; /* pointer to the next */
					     /* record in case we need */
					     /* to follow the */
					     /* 'triEdge' further to */
					     /* the present */
     int *spl;

{
  nextRecordType *futureNxt, *presentNxt;
  indexType futureIndex, presentIndex;
  indexType futureMaxEntries, presentMaxEntries;
  additionalNextRecordsType ** futureAdditional, ** presentAdditional;

  (*spl)++;

/*  printf("doLocatePoint: %d, %d, %d; %d at %d\n",
	 pORG (g, triEdge), pDST (g, triEdge), pDST (g, nxt[index].edge), p,
	 time);

  printPersistQe2 (nxt, index, maxEntries, additional);
*/
  if (*spl % 10 == 9) /* allitle heuristic to speed up the search */
    if ((*terminationCriterion) (g, pORG (g, triEdge), pDST (g, triEdge), p,
				 pDST (g, nxt[index].edge)))
      return -1;

  if (pqeISLASTENTRY (nxt, index, maxEntries, additional)) {
    /* found triangle of current triangulation which contains p */
    if (*spl % 10 != 9) /* compensation for the heuristic */
      if ((*terminationCriterion) (g, pORG (g, triEdge), pDST (g, triEdge), p,
				   pDST (g, nxt[index].edge)))
	return -1;
    
    return triEdge; 
  }

  pqeGetFuture (nxt, index, maxEntries, &additional, &futureNxt, &futureIndex,
		&futureMaxEntries, &futureAdditional);
		
  if (pqeISDEAD (futureNxt, futureIndex, futureMaxEntries,
		 *futureAdditional)) 
    /* triEdge has been flipped or deleted */

/*    printf("isdead\n");*/
    if (pqeISDEADslow (g, nxt[index].edge, futureNxt[futureIndex].time)) {
      /* edge is dead because of a 3-1 flip, the center of the flip is */
      /* the origin of the edge */
      pqeGetPresent (g, pOPREV (g, pSYM (triEdge), time), 
		     futureNxt[futureIndex].time,
		     &presentNxt, &presentIndex, &presentMaxEntries,
		     &presentAdditional);
/*      printf("halla %d\n", time);*/
      return doLocatePoint (g, pOPREV (g, pSYM (triEdge), time), p,
			    presentNxt[presentIndex].time, presentNxt,
			    presentIndex, presentMaxEntries, 
			    *presentAdditional, spl);
    } else 
      if (pqeISDEADslow (g, pONEXT (g, pSYM (nxt[index].edge), time),
			 futureNxt[futureIndex].time)) {
	/* edge is dead because of a 3-1 flip, the center of the flip is */
	/* the destination of the edge */
	pqeGetPresent (g, pSYM (nxt[index].edge), futureNxt[futureIndex].time,
		       &presentNxt, &presentIndex, &presentMaxEntries,
		       &presentAdditional);
/*	printf("hallo \n");*/
	return doLocatePoint (g, pSYM (nxt[index].edge), p,
			      presentNxt[presentIndex].time, presentNxt,
			      presentIndex, presentMaxEntries, 
			      *presentAdditional, spl);
      } else {
	/* edge is dead because of a 2-2 flip */
	pqeGetPresent (g, pSYM(nxt[index].edge), futureNxt[futureIndex].time,
		       &presentNxt, &presentIndex, &presentMaxEntries,
		       &presentAdditional);  
	return doubleTest (g, pSYM(nxt[index].edge), p,
			   presentNxt[presentIndex].time, presentNxt,
			   presentIndex, presentMaxEntries,
			   *presentAdditional, spl);
      }

  if (pqeISNEW (g, futureNxt[futureIndex].edge, futureNxt[futureIndex].time)) 
    if (pqeISNEW (g, pONEXT (g, pSYM(futureNxt[futureIndex].edge), 
			     futureNxt[futureIndex].time), 
		  futureNxt[futureIndex].time)) 
      /* new point was inserted inside the triangle */
      return tripleTest (g, triEdge, p, futureNxt[futureIndex].time, 
			 futureNxt, futureIndex, futureMaxEntries,
			 *futureAdditional, spl);
    else {
      /* pOPREV(SYM(triEdge)) was flipped */

/*      printf("pOPREV(SYM(triEdge)) was flipped\n");*/

      return doubleTest (g, triEdge, p, futureNxt[futureIndex].time,
			 futureNxt, futureIndex, futureMaxEntries,
                         *futureAdditional, spl);
    }

  if (pqeISNEW (g, pONEXT (g, pSYM(futureNxt[futureIndex].edge), 
			   futureNxt[futureIndex].time), 
		futureNxt[futureIndex].time)) {

    /* pONEXT(triEdge) was flipped */
    pqeGetPresent (g, pSYM(futureNxt[futureIndex].edge), 
		   futureNxt[futureIndex].time,
		   &presentNxt, &presentIndex, &presentMaxEntries,
		   &presentAdditional);
  
    return doubleTest (g, pSYM(futureNxt[futureIndex].edge), p,
		       presentNxt[presentIndex].time, presentNxt, 
		       presentIndex, presentMaxEntries,
		       *presentAdditional, spl);
  } else {
    /* 3-1 flip for which the destination of the nxt edge is the center */
     return doLocatePoint (g, triEdge, p, futureNxt[futureIndex].time, 
			  futureNxt, futureIndex, futureMaxEntries,
			  *futureAdditional, spl);
  }
}

/*---------------------------------------------------------------------------*/

static indexType
doubleTest (g, triEdge, p, time, nxt, index, maxEntries, additional, spl)

     graphType * g;
     indexType triEdge; /* triEdge is such that the flipped Edge is */
			/* pONEXT (g, triEdge, time) */
     indexType p;
     indexType time;
     nextRecordType *nxt;
     indexType index;
     indexType maxEntries; 
     additionalNextRecordsType * additional; 
     int *spl;

{
  nextRecordType *otherNxt;
  indexType otherIndex;
  indexType otherMaxEntries;
  additionalNextRecordsType ** otherAdditional;
  double dummy;

/*  printf ("double test\n");*/
/*printPersistQe (&(g->pe[pSYM(triEdge)]), pSYM(triEdge));
  printPersistQe (&(g->pe[triEdge]), triEdge);
*/
  if ((*leftTurnTest) (g, pDST (g, nxt[index].edge), pORG (g, triEdge), 
		       p, &dummy))
    return doLocatePoint (g, triEdge, p, time, nxt, index, maxEntries,
			  additional, spl);

  pqeGetPresent (g, nxt[index].edge, time, &otherNxt, &otherIndex,
		 &otherMaxEntries,  &otherAdditional);

  return doLocatePoint (g, nxt[index].edge, p, time, otherNxt,
			otherIndex, otherMaxEntries, *otherAdditional, spl);
}

/*---------------------------------------------------------------------------*/

static indexType
tripleTest (g, triEdge, p, time, nxt, index, maxEntries, additional, spl)

     graphType * g;
     indexType triEdge; /* triEdge is an edge of the boundary of the */
			/* triangle which contains the three */
			/* sub-triangles. triEdge is in ccw order on */
			/* the boundary. actually is on the boundary */
			/* of one of the subtriangles since here we */
			/* are already in the future. */
     indexType p;
     indexType time;
     nextRecordType *nxt;
     indexType index;
     indexType maxEntries;
     additionalNextRecordsType * additional;
     int *spl;

{
  nextRecordType *otherNxt;
  indexType otherIndex;
  indexType otherMaxEntries;
  additionalNextRecordsType ** otherAdditional;
  double dummy;

/*  printf ("triple test\n");*/

  if ((*leftTurnTest) (g, pDST (g, nxt[index].edge), pORG (g, triEdge), 
		       p, &dummy))
    if ((*leftTurnTest) (g, pDST (g, triEdge), pDST (g, nxt[index].edge), 
			 p, &dummy))
      return doLocatePoint (g, triEdge, p, time, nxt, index, maxEntries,
			    additional, spl);
    else {
      pqeGetPresent (g, pONEXT(g, pSYM(nxt[index].edge), time), time, &otherNxt,
		     &otherIndex, &otherMaxEntries, &otherAdditional);
      return doLocatePoint (g, pONEXT(g, pSYM(nxt[index].edge), time), p,
			    time, otherNxt, otherIndex, otherMaxEntries,
			    *otherAdditional, spl);
    }
  else
    if ((*leftTurnTest) (g, pDST (g, nxt[index].edge), 
			 pDST (g, pONEXT (g, nxt[index].edge, time)), 
			 p, &dummy)) {
      pqeGetPresent (g, nxt[index].edge, time, &otherNxt,
		     &otherIndex, &otherMaxEntries, &otherAdditional);
      return doLocatePoint (g, nxt[index].edge, p, time, otherNxt,
			    otherIndex, otherMaxEntries, *otherAdditional, spl);
    } else {
      pqeGetPresent (g, pONEXT(g, pSYM(nxt[index].edge), time), time, &otherNxt,
		     &otherIndex, &otherMaxEntries, &otherAdditional);
      return doLocatePoint (g, pONEXT(g, pSYM(nxt[index].edge), time), p,
			    time, otherNxt, otherIndex, otherMaxEntries,
			    *otherAdditional, spl);  
    }
}

/*---------------------------------------------------------------------------*/

indexType
hdagInsertPoint (g, p, localLeftTurnTest, localTerminationCriterion, spl)

     graphType * g;
     indexType p;
     int (*localLeftTurnTest) ();
     int (*localTerminationCriterion) ();
     int *spl;

/* inserts a point into the hdag. it returns the index of one of the */
/* quadedges inserted. the orientation of this edge is from one of the */
/* vertices of the triangle containing p to p. the return value is */
/* null when p cannot be inserted (due to 'localTerminationCriterion') */

{
  indexType triEdge;
  double dummy;

  triEdge = 
    hdagLocatePoint (g, p, localLeftTurnTest, localTerminationCriterion, spl);

  if (triEdge >= 0) 
    return pqeAddTriangleStar (g, triEdge, p);
  else
    return 0;
}

/*---------------------------------------------------------------------------*/

void
resetHdag (g)

     graphType * g;

{
  NE(g) = 0;
  g->time = TIME_0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
