/*
 * NAME: graph.c
 *
 * DESCRIPTION:
 *	This file contains routines for manipulating 2-D alpha shape
 *	graphs, using the quad-edge data structure to represent the
 *	edge topology.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/12/06: gphUniqSites added
 */

#include <stdio.h>
#include <limits.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"
#include "math.h"

#define ERRFILE stdout
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

//extern char *malloc(), *calloc();
extern void quickSortIndex();
extern void dumpGraph();

graphType *globalG;
int gphDebug = 0;

void
printGraphInfo (g)
     
     graphType *g;

{
  printf ("Graph Statistics: sites %d, edges %d, ch-edges %d,\n", NOS (g), NE (g), g->nChEdges);
  printf ("                  sites not in triangulation %d.\n", (NOS (g) * 3 - 3 - g->nChEdges - NE(g)) / 3);
}


graphType * newGraph ()

{
  graphType *g;

  if ((g = (graphType *) malloc(sizeof(graphType))) == NULL) {
    (void) fprintf(ERRFILE, "gphNew: fail of malloc.\n");
    exit(1);
  }

  NS(g) = NOS (g) = NE(g) = NF(g) = NA(g) = g->naRational = 0;
  g->maxs = g->maxe = g->maxf = g->maxa = 0;
  g->nChEdges = g->nDtcEdges = g->nDtcFacets = -1;
  g->chEdge = 1;
  g->alpha = g->a0 = -1;
  g->s = NULL;
  g->e = NULL;
  g->pe = NULL;
  g->f = NULL;
  g->a = NULL;
  g->adbl = NULL;
  g->xMin = INT_MAX; 
  g->yMin = INT_MAX; 
  g->xMax = INT_MIN; 
  g->yMax = INT_MIN;
  g->zMin = INT_MAX; 
  g->zMax = INT_MIN;

/*
  P_-2           P_-1
    *<------------*
     \           /
      \    0    /
       \       /
        \     /
         \   /
          \ /
           * P_-3
*/

/* infP [i] corresponds to P_-i */

  g->infP[1][0] = 1;
  g->infP[1][1] = 1;
  g->infP[2][0] = -1;
  g->infP[2][1] = 1;
  g->infP[3][0] = 0;
  g->infP[3][1] = -1;

  return(g);
}  /* -------------------- gphNew -------------------- */


void gphDispose(g)
graphType *g;
{
  if (NS(g)) free((char *) g->s);
  if (NE(g)) free((char *) g->e);
  if (NF(g)) free((char *) g->f);
  if (NA(g)) free((char *) g->a);

  if (g->adbl != NULL) free((char *) g->adbl);

}  /* -------------------- gphDispose -------------------- */


void gphAllocate(g, maxs, maxe, maxf, maxa)
graphType *g;
int maxs, maxe, maxf, maxa;
/*
 *   Allocate space for non-zero objects.
 */
{
  int i;

  if (gphDebug) (void)
    printf("gphAllocate: ns %d, ne %d, nf %d, na %d\n", maxs,maxe,maxf,maxa);

  if (maxs) {
    if (NS(g)) (void)
      fprintf(ERRFILE, "gphAllocate: Warning: sites already allocated.\n");
    g->s = (siteType *) calloc((unsigned int) maxs, sizeof(siteType));
    if (g->s == NULL) (void)
      fprintf(ERRFILE, "gphAllocate: fail on site allocation.\n");
    NS(g) = 0;
    g->maxs = maxs;
  }

  if (maxe) {
    if (NE(g)) (void)
      fprintf(ERRFILE, "gphAllocate: Warning: quadedges already allocated.\n");
    g->e = (qe1Type *) calloc((unsigned int) maxe, 4 * sizeof(qe1Type));
    if (g->e == NULL) (void)
      fprintf(ERRFILE, "gphAllocate: fail on quadedge allocation.\n");
    NE(g) = 0;
    g->maxe = maxe;

    maxe *= 4;		/* number of edges to number of quadedges */
    for (i=0; i < maxe; i++) {
      g->e[i].dat = -1;
      g->e[i].nxt = -1;
    }
  }

  if (maxf) {
    if (NF(g)) (void)
      fprintf(ERRFILE, "gphAllocate: Warning: facets already allocated.\n");
    g->f = (facetType *) calloc((unsigned int) maxf, sizeof(facetType));
    if (g->f == NULL) (void) 
      fprintf(ERRFILE, "gphAllocate: fail on facet allocation.\n");
    NF(g) = 0;
    g->maxf = maxf;
  }

  if (maxa) {
    if (NA(g)) (void)
      fprintf(ERRFILE, "gphAllocate: Warning: a values already allocated.\n");

    g->a = (alphaType *) calloc((unsigned int) maxa, sizeof(alphaType));
    if (g->a == NULL) (void)
      fprintf(ERRFILE, "gphAllocate: fail on alpha values allocation.\n");
    NA(g) = 0;
    g->maxa = maxa;

    g->adbl = (double *) calloc((unsigned int) maxa, sizeof(double));
    if (g->adbl == NULL) (void)
      fprintf(ERRFILE, "gphAllocate: fail on alpha doubles allocation.\n");
  }
}  /* -------------------- gphAllocate ------------------- */


void gphLoadSite(g, x, y, z)
graphType *g;
coordType x, y, z;
/*
 *   Load an entry into the site table along with an alpha index.
 */
{
  if (gphDebug) {
#ifdef FLOATPOINT
    (void) printf("gphLoadSite: site %d: (%lf, %lf), aindex: %d\n",
      NS(g), x, y, a);
#else
    (void) printf("gphLoadSite: site %d: (%d, %d, %d)\n", NS(g), x, y, z);
#endif
  }

  if (NS(g) < g->maxs) {
    SITEX (g, NS(g)) = x;
    SITEY (g, NS(g)) = y;
    SITEZ (g, NS(g)) = z;
    SITENset (g, NS(g), NS(g));
    SITEP (g, NS(g)) = SITEN (g, NS(g));
    NS(g) += 1;
    NOS(g) += 1;
    g->xMin = MIN (g->xMin, x);
    g->xMax = MAX (g->xMax, x);
    g->yMin = MIN (g->yMin, y);
    g->yMax = MAX (g->yMax, y);
    g->zMin = MIN (g->zMin, z);
    g->zMax = MAX (g->zMax, z);
  }

  else
    (void) fprintf(ERRFILE, "gphAddSite: sites overflow.\n");
}  /* -------------------- gphLoadSite ------------------- */


void gphLoadEdge(g, org, dst, amin, amax)
graphType *g;
int org, dst, amin, amax;
/*
 *   gphLoadEdge loads edge information into the quadedge data structure.
 * Note that only the org and dst sites and the indices for amin and
 * amax are loaded, i.e. all the nxt fields are left undefined.
 */
{
  indexType newqe = MAKEQE(NE(g));

  if (NE(g) < g->maxe) {
    SETORG(g, newqe, org);
    SETDST(g, newqe, dst);
    SETEMINA(g, NE(g), amin);
    SETEMAXA(g, NE(g), amax);
    NE(g) += 1;
  }

  else
    (void) fprintf(ERRFILE, "gphLoadEdge: edge overflow.\n");
}  /* -------------------- gphLoadEdge -------------------- */


void gphLoadFacet(g, e0, e1, e2, a)
graphType *g;
int e0, e1, e2, a;
/*
 *   This routine updates the facet table, given the edges and alpha index
 * defining a particular face.  This would typically called by the
 * routine graphRead.
 */
{
  if (NF(g) < g->maxf) {
    g->f[NF(g)].e[0] = e0;
    g->f[NF(g)].e[1] = e1;
    g->f[NF(g)].e[2] = e2;
    g->f[NF(g)].a = a;
    NF(g) += 1;
  }

  else
    (void) fprintf(ERRFILE, "gphAddFacet: facet overflow.\n");
}  /* ------------------- graphLoadFacet ------------------- */


void gphLoadAlpha(g, adbl)
graphType *g;
double adbl;
/*
 *   Load a floating point representation of an alpha value into the
 * the alpha table (note that rational representation is not defined).
 */
{
  if (NA(g) < g->maxa) {

#ifdef FLOATPOINT
    g->a[NA(g)] = adbl;
#else
    g->adbl[NA(g)] = adbl;
#endif

    NA(g) += 1;
  }

  else
    (void) fprintf(ERRFILE, "gphLoadAlpha: alpha values overflow.\n");
}  /* ------------------ gphLoadAlpha -------------------- */


int gphAssignAlpha(g, alpha)
graphType *g;
alphaType *alpha;
/*
 *   Copy an entry into the alpha table.
 */
{
  int entryNumber;

  if (NA(g) >= g->maxa) {
    (void) fprintf(ERRFILE, "gphAssignAlpha: out of space for alpha values.\n");
    return(-1);
  }
 
#ifdef FLOATPOINT
  g->a[NA(g)] = *alpha;    
#else
  lmCopy(&alpha->num, &g->a[NA(g)].num);
  lmCopy(&alpha->den, &g->a[NA(g)].den);
  g->naRational += 1;
#endif

  entryNumber = NA(g);
  NA(g) += 1;
  return(entryNumber);
}  /* -------------------- gphAssignAlpha -------------------- */


void gphAssignFacet(g, alpha, qe0, qe1, qe2)
graphType *g;
alphaType *alpha;
indexType qe0, qe1, qe2;
/*
 *   gphAssignFacet updates the quadedge, facet and alpha tables, given the 
 * (double or rational) alpha for the facet and the three ccw quadedges
 * surrounding the it.  This routine differs from gphLoadFacet in that
 * the load routine only modifies the facet table.
 */
{
  int aindex;

  if (NF(g) >= g->maxf) {
    (void) fprintf(ERRFILE, "gphAssignFacet: out of facet space.\n");
    return;
  }

  aindex = gphAssignAlpha(g, alpha); 

  /* mark the edges adjacent to the facet */
  SETLEFTFACET(g, qe0, aindex);
  SETLEFTFACET(g, qe1, aindex);
  SETLEFTFACET(g, qe2, aindex);

  /* update the facet table */
  g->f[NF(g)].e[0] = QETOE(qe0);
  g->f[NF(g)].e[1] = QETOE(qe1);
  g->f[NF(g)].e[2] = QETOE(qe2);
  g->f[NF(g)].a = NF(g);

  NF(g) += 1;
}  /* ------------------- gphAssignFacet ------------------ */



void gphAssignSiteAlpha(g, site, aindex)
graphType*g;
int site, aindex;
{
  if (site > NS(g)) {
    (void) fprintf(ERRFILE, "gphSetSiteAlpha: site out of range.\n");
    return;
  }

  g->s[site].a = aindex;
}  /* -------------------- gphSetSiteAlpha -------------------- */


int ltAmap(aMap, a1, a2)
indexType aMap[];
int a1, a2;
{
  return(lmRatLt(&globalG->a[aMap[a1]].num,
                 &globalG->a[aMap[a1]].den,
                 &globalG->a[aMap[a2]].num,
                 &globalG->a[aMap[a2]].den));

}  /* -------------------- ltAmap -------------------- */


void swapAmap(aMap, a1, a2)
indexType aMap[];
int a1, a2;
{
  indexType tmp;

  tmp = aMap[a1];
  aMap[a1] = aMap[a2];
  aMap[a2] = tmp;

}  /* ------------------- swapAmap ------------------- */


/*   This routine sorts entries in the alpha array, eliminating duplicates.
 * A map of old indicies to new indicies is used to update alpha references
 * for sites, edges and facets.
 */
void gphSortUniqAlpha(g)
graphType *g;
{
  indexType *mapnto;			/* map new to old */
  indexType *mapotn;			/* map old to new */
  alphaType *newAlphaTable, *oldAlphaTable, anum, aden;
  int i, isrc, idst, naNew, showSortA = 0;
  double a2;

  (void) printf("sortAlpha: ");  (void) fflush(stdout);

  /* allocate space for maps between old alpha numbering and new */
  mapnto = (indexType *) calloc((unsigned int) NA(g), sizeof(indexType));
  mapotn = (indexType *) calloc((unsigned int) NA(g), sizeof(indexType));
  if (mapotn == NULL || mapnto == NULL) {
    (void) fprintf(stdout, "sortAlpha: fail on calloc.\n");
    exit(1);
  }


  /* initialize new to old map */
  for (i=0; i < NA(g); i++) mapnto[i] = i;

  /* generate a map from new to old using quicksort */
  globalG = g;
  quickSortIndex(mapnto, NA(g), ltAmap, swapAmap);
  (void) printf("%d alpha sorted, ", NA(g));  (void) fflush(stdout);

  /*   Generate a map from old to new while also "uniquifying" alphas.
   * The new-to-old map is one-to-one onto.  The old-to-new map potentially
   * will be many to one and not onto.
   */
  isrc = idst = 0;
  while (isrc < NA(g)) {
    lmCopy(&g->a[mapnto[isrc]].num, &anum);	/* get copy of alpha */
    lmCopy(&g->a[mapnto[isrc]].den, &aden);

    while (lmRatEq(&anum, &aden, &g->a[mapnto[isrc]].num,
        &g->a[mapnto[isrc]].den)) {
      mapotn[mapnto[isrc]] = idst;
      isrc++;
      if (isrc >= NA(g)) break;
    }
    idst++;
  }
  naNew = idst;					/* number of unique alpha */
  (void) printf("%d unique alpha, remapping ... ", naNew);
  (void) fflush(stdout);

  if (showSortA)
    for (i=0; i < NA(g); i++)
      (void) printf("sortAlpha: mapotn[%d] = %d\n", i, mapotn[i]);

  free((char *) mapnto);

  /* change all the alpha references for facets, edges and sites */
  for (i=0; i < NF(g); i++) 
    FACETA(g, i) = mapotn[FACETA(g, i)];

  for (i=0; i < NE(g); i++) {
    SETEMINA(g, i, mapotn[EMINA(g, i)]);
    SETEMAXA(g, i, mapotn[EMAXA(g, i)]);
  }
  for (i=0; i < NS(g); i++)
    SITEA(g, i) = mapotn[SITEA(g, i)];

  /* replace alpha values with a sorted version */
  newAlphaTable = (alphaType *) calloc((unsigned int) naNew,sizeof(alphaType));
  if (newAlphaTable == NULL) {
    (void) fprintf(stdout, "sortAlpha: calloc failed.\n");
    exit(1);
  }

  for (i=0; i < NA(g); i++) {
    lmCopy(&g->a[i].num, &newAlphaTable[mapotn[i]].num);
    lmCopy(&g->a[i].den, &newAlphaTable[mapotn[i]].den);
  }
  oldAlphaTable = g->a;
  g->a = newAlphaTable;
  NA(g) = naNew;

  free((char *) oldAlphaTable);
  free((char *) mapotn);

#ifdef FLOATPOINT
  g->adbl = g->a;	/* alpha terms already represented as reals */

#else

  /* calculate floating point representation of alpha values (w/ sqrt) */
  g->adbl = (double *) calloc((unsigned int) NA(g), sizeof(double));
  if (g->adbl == NULL) (void)
    fprintf(ERRFILE, "gphSortAlpha: fail on alpha double allocation.\n");

  for (i=0; i < NA(g); i++) {
    a2 = lmReal(&g->a[i].num) / lmReal(&g->a[i].den);
    if (a2 < 0)
      g->adbl[i] = - sqrt(-a2);
    else
      g->adbl[i] =  sqrt( a2);
  }
#endif

  (void) printf(" done\n");
}  /* -------------------- gphSortUniqAlpha -------------------- */


#ifdef FLOATPOINT
#define EPSILON 0.0001
#define EQ(x1, x2) ((x1)-(x2) < EPSILON  &&  (x2)-(x1) < EPSILON)
#else
#define EQ(x1, x2) ((x1) == (x2))
#endif

/*
 *   gphUniqSites remove duplicate sites from the array in the graphType
 * pointed to by g.  The sites are assumed to already be sorted.  This
 * routine should not be called if there are already edges or facets
 * referring to the sites.
 */

void 
gphUniqSites(g, swap)

     graphType *g;
     void (*swap)();

{
  int idst = 0, isrc = 0;

  while (isrc < NS(g)) {
    swap (g->s, idst, isrc);
    while (++isrc < NS(g)) {
      if (SITEX(g, idst) != SITEX(g, isrc)) break;
      if (SITEY(g, idst) != SITEY(g, isrc)) break;
    }
    idst++;
  }

  /* if new number of sites not equal to old (i.e. there were duplicates) */
  if (idst != NS(g)) {
    if (NE(g) || NF(g))  (void) printf(
      "gphUniqSites: Warning! Edge or facet references could be corrupted.\n");
    (void) printf("gphUniqSites: %d => %d sites, %d duplicates found.\n",
      NS(g), idst, NS(g) - idst);
  } else
    (void) printf("gphUniqSites: no duplicate Points in data.\n");

  NS(g) = idst;
}  /* ------------------- gphUniqSites -------------------- */

/*---------------------------------------------------------------------------*/

void swapSite (siteTbl, s1, s2)
siteType siteTbl[];
int s1, s2;
{
  coordType tmp;
  indexType tmpI;


  tmp = siteTbl[s1].coord[0];  
  siteTbl[s1].coord[0] = siteTbl[s2].coord[0];  
  siteTbl[s2].coord[0] = tmp;
  tmp = siteTbl[s1].coord[1];  
  siteTbl[s1].coord[1] = siteTbl[s2].coord[1];  
  siteTbl[s2].coord[1] = tmp;
  tmp = siteTbl[s1].coord[2];  
  siteTbl[s1].coord[2] = siteTbl[s2].coord[2];  
  siteTbl[s2].coord[2] = tmp;
  tmpI = siteTbl[s1].name;
  siteTbl[s1].name = siteTbl[s2].name;  
  siteTbl[s2].name = tmpI;
  siteTbl[siteTbl[s1].name].position = s1;
  siteTbl[siteTbl[s2].name].position = s2;

}

/*---------------------------------------------------------------------------*/

