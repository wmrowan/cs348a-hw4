/*
 * NAME: graph.h
 *
 * DESCRIPTION:
 *	Within this header file are the definitions for the representation
 *	of the alpha graphs and the macro definitions used to access them.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/12/06: gphUniqSites prototype added
 *	89/12/07: gphSortAlpha() to gphSortUniqAlpha()
 *
 * NOTE:
 *	Notice that this header is dependant upon the longmath.h
 *	and quadedge.h header files, and will include them if they haven't
 *	already been included previous to this file.
 */

#ifndef QEINCLUDED
#include "quadedge.h"
#endif

#ifndef LMINCLUDED
#include "longmath.h"
#endif

typedef struct {
  lmNumberType num, den;
} ratType;

#ifdef FLOATPOINT
  typedef double alphaType;
#else
  typedef ratType alphaType;
#endif

typedef struct {
  coordType coord[3];
  indexType a;
  indexType name; /* name of the vertex stored in this position */
  indexType position; /* index of the vertex with this name */
} siteType;

typedef struct {
  indexType e[3];
  indexType a;
} facetType;

typedef struct {

  /* some additions for the persistent quadedge. what a hack */
  indexType     time; /* this is actually a count of the number of */
		      /* quadedge changes performed (used for persistant) */
  indexType     topTriangleEdge;
  persistQeType *pe;
  coordType     infP[4][2];

  siteType	*s;
  qe1Type	*e;
  facetType	*f;
  alphaType	*a;
  double	*adbl;
  indexType 	ns, nos, ne, nf, na, naRational;
  indexType	maxs, maxe, maxf, maxa;
  indexType	chEdge, nChEdges, nDtcEdges, nDtcFacets;
  indexType	alpha;		/* alpha interval to draw */
  indexType	a0;		/* alpha interval from - to + */
  coordType     xMin, xMax, yMin, yMax, zMin, zMax;
  char * fileName;

} graphType;

#define NS(g) (g)->ns			/* number of sites */
#define NOS(g) (g)->nos			/* number of original sites */
  /* NS != NOS if some sites are removed, e.g. duplicates */
#define NE(g) (g)->ne			/* number of edges */
#define NF(g) (g)->nf			/* number of facets */
#define NA(g) (g)->na			/* number of alpha values */

#define SITEX(g, site) (g)->s[site].coord[0]
#define SITEY(g, site) (g)->s[site].coord[1]
#define SITEZ(g, site) (g)->s[site].coord[2]
#define COORDVECT(g, site) (g)->s[site].coord
#define SITEA(g, site) (g)->s[site].a
#define SITEN(g, site) (site >= 0 ? (g)->s[site].name : site)
#define SITENset(g, site, siteName) do {(g)->s[site].name = siteName;} while (0)
#define SITEP(g, site) (g)->s[site].position

#define FACETA(g, facet) (g)->f[facet].a

/* function prototypes */
extern graphType *	newGraph();
extern void		gphDispose();
extern void		gphAllocate();
extern void		gphLoadSite();
extern void		gphLoadEdge();
extern void		gphLoadFacet();
extern void		gphLoadAlpha();
extern int		gphAssignAlpha();
extern void		gphAssignSiteAlpha();
extern void		gphAssignFacet();
extern void		gphSortUniqAlpha();
extern void		gphUniqSites();

/*---------------------------------------------------------------------------*/

