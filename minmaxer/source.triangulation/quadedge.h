/*
 * NAME: quadedge.h
 *
 * DESCRIPTION:
 *	This file contains type definitions and procedure prototypes
 *	supporting the quadedge data structure.
 *
 * HISTORY:
 *	89/10/26: This header added.
 *
 * SEE ALSO:
 *	Guibas, L. and Stolfi, J. "Primitives for the Manipulation of
 *	General Subdivisions and the Computation of Voronoi Diagrams"  ACM
 *	Transactions on Graphics, Vol. 4, No. 2, April 1985, pp. 74-123.
 */

#define QEINCLUDED 1

/* typedef short int indexType; */
typedef struct {
  unsigned int isCH : 1;
  unsigned int isConstrained : 1;
  unsigned int isFinal : 1;
  unsigned int isDeleted : 1;
} edgeStatusType;

typedef struct {
  edgeStatusType status;
  indexType      dat, nxt;
} qe1Type;

extern indexType QNBR (/*qe*/);
extern indexType MAKEQE (/*e*/);
extern indexType QETOE (/*qe*/);
extern indexType QETOQE0 (/*qe*/);

extern indexType SYM (/*qe*/);
extern indexType ORG (/*g, qe*/);
extern indexType DST (/*g, qe*/);

extern void SETORG (/*g, qe, org*/);
extern void SETDST (/*g, qe, dst*/);

extern indexType ROT1 (/*qe*/);
extern indexType ROT3 (/*qe*/);

extern indexType ONEXT (/*g, qe*/);
extern indexType OPREV (/*g, qe*/);

extern void SETONEXT (/*g, qe, next*/);

extern indexType LNEXT (/*g, qe*/);
extern indexType RNEXT (/*g, qe*/);

extern indexType EMINA (/*g, n*/);
extern indexType EMAXA (/*g, n*/);

extern indexType LEFTFACET (/*g, qe*/);

extern void SETEMINA (/*g, n, pt*/);
extern void SETEMAXA (/*g, n, pt*/);

extern void SETLEFTFACET (/*g, qe, facet*/);


extern int ISCHQE (/*g, qe*/);
extern void SETCHQE (/*g, qe*/);
extern void UNSETCHQE (/*g, qe*/);

extern int ISCONSTRQE (/*g, qe*/);
extern void SETCONSTRQE (/*g, qe*/);
extern void UNSETCONSTRQE (/*g, qe*/);

extern int ISFINALQE (/*g, qe*/);
extern void SETFINALQE (/*g, qe*/);
extern void UNSETFINALQE (/*g, qe*/);


extern int ISDELETEDQE (/*g, qe*/);
extern void SETDELETEDQE (/*g, qe*/);
extern void UNSETDELETEDQE (/*g, qe*/);

extern int ISCHedge (/*g, edge*/);
extern void SETCHedge (/*g, edge*/);
extern void UNSETCHedge (/*g, edge*/);

extern int ISCONSTRedge (/*g, edge*/);
extern void SETCONSTRedge (/*g, edge*/);
extern void UNSETCONSTRedge (/*g, edge*/);

extern int ISFINALedge (/*g, edge*/);
extern void SETFINALedge (/*g, edge*/);
extern void UNSETFINALedge (/*g, edge*/);

extern int ISDELETEDedge (/*g, edge*/);
extern void SETDELETEDedge (/*g, edge*/);
extern void UNSETDELETEDedge (/*g, edge*/);


/* function prototypes */
extern indexType	qeAddSiteSite();
extern indexType	qeAddEdgeSite();
extern indexType	qeAddEdgeEdge();
extern void		qeFlip();
extern void		qeRenumberEdges();
extern void		qeChCopy();
extern void		qeMeld();
extern void		qeDump();
extern int		qeVerify();
extern void		qeReserveEdges();
extern void             qeDelete ();
extern void             qeReinsert ();
extern void             qeResetCHstatus();
extern void             qeSetCHstatus();
extern void             qeResetCONSTRstatus();
extern void             qeResetFINALstatus();
extern void             qeSetDELETEDstatus();
extern void             qeResetDELETEDstatus();
extern void             qeResetALLstati();
extern void             qeDuplicate ();

#include "persistent.quadedge.h"
