/*
 * NAME: texput.c
 *
 * DESCRIPTION:
 *	Texput reads in a graph from a file (in the format expected by
 *	the routines in file_io.c) and writes to stdout commands suitable
 *	for inclusion in a LaTeX document using the PicTeX macros.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/12/10: Graceful end if no filename specified
 *	89/12/10: debugging toggle added, fillFacet site bug fixed
 *
 * SEE ALSO:
 *	M. Wichura, "The PicTeX Manual," 1987.
 */


#include <stdio.h>
#include "graph.h"
#include "file_io.h"

#define MARGIN 0.0

extern int drawDt;

void quickSortIndex() {};

extern void processArgs();
FILE *fp = stdout;

int tpDebug=0;
/***********/

#define MAXV 256			/* max number of vertices allowed */
#define MAXE 32				/* max edges within any slab */

#define EPSILON 0.001
#define EQ0(x) (((x) > -(EPSILON)) && ((x) < EPSILON))
#define DIV2(x) ((x) * 0.5)

debug = 0;				/* set to a nonzero value to debug */


typedef struct {
  int vl, vr;				/* left and right vertices of edge */
  double m, b;				/* parameters for edge line y=mx+b */
} edgetype;

typedef struct {
  double x,y;
} verttype;

int polyfill(v, nv)
verttype v[];				/* array of vertices */
int nv;					/* number of vertices */
{
  static int sv[MAXV];
  static edgetype e[MAXE];
  int ne = 0;
  static int se[MAXE];
  
  register int i, j;
  double xl, xr, yll, ylu, yrl, yru, xmed, width;
  void sortv(/* v, nv, sv */);
  int update_e(/* v, v0, v1, e, ne */);
  void sorte(/* xmed, e, ne, se */);

  if (tpDebug) {
    (void) printf("polyfill: ");
    for (i=0; i < nv; i++)
      (void) printf("(%d,%d) ", (int) v[i].x, (int) v[i].y);
    (void) printf("\n");
  }

  if (nv < 3) return(0);		/* could not have area */
  if (nv > MAXV) return(-1);		/* too many vertices to handle */

  sortv(v, nv, sv);			/* sort the vertices */

  /* plane sweep, with each vertex an event point from left to right */
  for (i=0; i<(nv-1); i++) {
    /* update table of edges passing through sweep slab */
    if (update_e(v, sv[i], ((sv[i] + 1) % nv), e, &ne)) return(-2);
    if (update_e(v, sv[i], ((sv[i] - 1 + nv) % nv), e, &ne)) return(-2);

    width = v[sv[i+1]].x - v[sv[i]].x;	/* get sweep slab width */
    /* if sweep slab is zero width, skip */
    if (EQ0(width)) continue;

    /* figure x values for left and right and middle of slab */
    xl = v[sv[i]].x;
    xr = v[sv[i+1]].x;
    xmed = DIV2(xl + xr);

    /* ensure that the edges in the edge table are sorted */
    sorte(xmed, e, ne, se);

    /* print out PicTeX commands for area fill(s) */
    for (j=0; j<ne; j+=2) {
      ylu = e[se[j+1]].m*xl + e[se[j+1]].b;
      yll = e[se[j  ]].m*xl + e[se[j  ]].b;
      yru = e[se[j+1]].m*xr + e[se[j+1]].b;
      yrl = e[se[j  ]].m*xr + e[se[j  ]].b;
      printf("  \\vshade %lf %lf %lf  %lf %lf %lf /\n",
        xl, yll, ylu, xr, yrl, yru);
    }
  }
  return(0);				/* 0 indicates no error */
}  /* -------------------- polyfill -------------------- */

/*   Sortv calculates the values for the sv array.  These indices
 * specify the vertices in ascending x order.  The algorithm is similar
 * to insertion sort, where values are taken from the unsorted set and
 * added to the sorted set.  The newly entered value must "sink" to its
 * proper place in the sorted array so that the values will remain sorted.
 */
void sortv(v, nv, sv)
verttype v[];
int nv;
int sv[];
{
  register i=0, j=0, i1, i2;
  int tmp;

  /* find the vertex in v[] with the max x value */
  for (j=1; j<nv; j++) 
    if (v[j].x > v[i].x) i = j;		/* save index of vertex in i */

  /* i1 and i2 are indices of vertices to be added to the sorted array sv[] */
  i1 = (i-1 + nv) % nv;
  i2 = (i+1) % nv;
  sv[--j] = i;				/* sv[nv-1] = i */
  j--;					/* j = nv-2 */

  /* loop, adding each vertex to sorted array sv[] */
  while (j>=0) {
    /* i1 and i2 are indices of the insertion candidates */
    if (v[i1].x > v[i2].x) {
      sv[j] = i1;
      if (--i1 < 0) i1 += nv;		/* decrement and wrap around */
    }
    else {				/* v[i2] has greater x */
      sv[j] = i2;
      if (++i2 == nv) i2 = 0;		/* increment and wrap around */
    }

    /* sink the newly added value to its proper place as in insertion sort */
    i = j;
    while (v[sv[i]].x > v[sv[i+1]].x) {
      /* swap sv[i] and sv[i+1] */
      tmp = sv[i+1]; sv[i+1] = sv[i]; sv[i] = tmp;
      i++;
    }
  j--;
  }
  if (debug)
    for (i=0; i<nv; i++)
      printf("sv[%d]: v[%d]: (%7.2lf,%7.2lf)\n", 
        i, sv[i], v[sv[i]].x, v[sv[i]].y);
}  /* -------------------- sortv -------------------- */

/*   Update_e updates the table of vertices (e[]) given the two vertices
 * of an edge.  v0 is assumed to have the same x value as the sweep line
 * (i.e. the left side the currently considered slab).  If the x value
 * of vertex v1 is greater than that of v0, then the sweep line is
 * encountering a new edge, and the edge must be added to the edge table.
 * Otherwise, if the x value of v1 is less than that of v0, then the
 * sweep line is at the end of an edge, and it should be removed from
 * the table.  Note that vertical edges are ignored.  Note also that
 * if an edge is inserted into the table, the sorting in the array se[]
 * is no longer valid, in other words the routine sorte needs to be used.
 */
int update_e(v, v0, v1, e, ne)
verttype v[];
int v0, v1;				/* vertices of edge */
edgetype e[];				/* table of edges */
int *ne;				/* number of edges in table */
{
  register int i;
  double m, b;

  /* if edge is vertical, skip it */
  if (EQ0(v[v0].x - v[v1].x)) return(0);

  /* if the sweep line is at the beginning of an edge, add it to table */
  if (v[v0].x < v[v1].x) {
    /* if space for edges would be exceeded, return an error value */
    if (*ne == MAXE) return(-1);

    if (debug) {
      printf("adding edge from (%6.2lf,%6.2lf) to (%6.2lf,%6.2lf)",
        v[v0].x, v[v0].y, v[v1].x, v[v1].y);
      printf(" (edge %d)\n", *ne);
    }

    /* figure parameters (y = m*x + b) of the edge line */
    m = (v[v1].y - v[v0].y) / (v[v1].x - v[v0].x);
    b = v[v0].y - m * v[v0].x;

    /* add entry to end of table */
    e[*ne].vl = v0;
    e[*ne].vr = v1;
    e[*ne].m = m;
    e[*ne].b = b;
    (*ne)++;				/* number of edges goes up by 1 */
  }
  else {
    /* else, sweep line at right end of edge, remove it */
    if (debug)
      printf("removing edge from (%6.2lf,%6.2lf) to (%6.2lf,%6.2lf)\n",
        v[v0].x, v[v0].y, v[v1].x, v[v1].y);

    /* find where the edge is within the table */
    for (i=0; i< *ne; i++)
      if ((v1 == e[i].vl) && (v0 == e[i].vr)) break;

    /* move the other entries within the table down one */
    (*ne)--;
    for ( ; i< *ne; i++) {
      e[i].vl = e[i+1].vl;
      e[i].vr = e[i+1].vr;
      e[i].m = e[i+1].m;
      e[i].b = e[i+1].b;
    }
  }
  return(0);				/* return value indicating no error */
}  /* -------------------- update_e -------------------- */

/*   Sorte sorts the indices in the array se[] so that the records in
 * the edge table can be accesssed in ascending order within 
 * the slab for which xmed is in the middle.  The technique is a 
 * variant of insertion sort.
 */
void sorte(xmed, e, ne, se)
double xmed;				/* value within the slab */
edgetype e[];				/* table of edges */
int ne;					/* number of edges */
int se[];				/* array of indices for sorting */
{
  register int i, j;
  int tmp;

  /* initially, ordering of edge records taken to be as they are */
  for (i=0; i<ne; i++) se[i] = i;

  for (i=1; i<ne; i++) {
    /* edges 0 through i-1 sorted */
    for (j=i; j>0; j--) {
      if ((e[se[j]].m*xmed + e[se[j]].b) > (e[se[j-1]].m*xmed + e[se[j-1]].b)) 
        break;
      else 
        /* swap indices se[j] and se[j-1] */
        tmp = se[j]; se[j] = se[j-1]; se[j-1] = tmp;
    }
    /* edges 0 through i sorted */
  }
  if (debug) {
    printf(" e[se[]] vl   vr    m      b\n");
    for (i=0; i<ne; i++) 
      printf("   %3d: %3d  %3d  %5.2lf  %5.2lf\n",
        i, e[se[i]].vl, e[se[i]].vr, e[se[i]].m, e[se[i]].b);
  }
}  /* -------------------- sorte -------------------- */


/**********/

static void calcWindowSize(g, llx, lly, urx, ury)
graphType *g;
double *llx, *lly, *urx, *ury;
{
  indexType i;
  coordType px, py;
  float xmargin, ymargin;
 
  *llx = *urx = SITEX(g, 0);
  *lly = *ury = SITEY(g, 0);
  for (i=1; i< NS(g); i++) {
    px = SITEX(g, i);  py = SITEY(g, i);
    if (px < *llx)
      *llx = px;
    else if (px > *urx)
      *urx = px;

    if (py < *lly)
      *lly = py;
    else if (py > *ury)
      *ury = py;

  }
  /* (void) fprintf(stdout, 
       "calcWindowSize: llx %lf lly %lf urx %lf ury %lf\n",
       *llx, *lly, *urx, *ury);
    (void) fflush(stdout);
  */

  xmargin = MARGIN * (float) (*urx - *llx);
  ymargin = MARGIN * (float) (*ury - *llx);
  if (xmargin < 1) xmargin = 2;
  if (ymargin < 1) ymargin = 2;

  *llx -= xmargin;  *lly -= ymargin;
  *urx += xmargin;  *ury += ymargin;
}  /* -------------------- calcWindowSize -------------------- */


static void fillFacet(g, i)
graphType *g;
int i;
{
  int s0, s1, s2;
  static verttype v[3];

  s0 = ORG(g, MAKEQE(g->f[i].e[0]));
  s1 = DST(g, MAKEQE(g->f[i].e[0]));
  s2 = ORG(g, MAKEQE(g->f[i].e[2]));
  if (s2 == s0 || s2 == s1)
    s2 = DST(g, MAKEQE(g->f[i].e[2]));

  if (tpDebug) {
    (void) printf("fillFacet: edges %d %d %d\n", g->f[i].e[0],
      g->f[i].e[1], g->f[i].e[2]);
    (void) printf("fillFacet: sites %d %d %d\n", s0, s1, s2);
  }

  v[0].x = SITEX(g, s0);  v[0].y = SITEY(g, s0);
  v[1].x = SITEX(g, s1);  v[1].y = SITEY(g, s1);
  v[2].x = SITEX(g, s2);  v[2].y = SITEY(g, s2);

  (void) polyfill(v, 3);

}  /* ------------------- fillFacet ------------------- */


void writePicTeX(g)
graphType *g;
{
  int i;
  int a = g->alpha;
  indexType qe, org, dst;
  double llx, lly, urx, ury, span, units;

  calcWindowSize(g, &llx, &lly, &urx, &ury);
  if ((span = urx - llx) < 0.0001)
    (void) fprintf(stdout, "writePicTex: warning: very small urx-llx span.\n");
  units = 5.0 / span;

  /* do coordinate system shift to get all values positive */
  for(i=0; i < NS(g); i++) {
    SITEX(g, i) -= llx;
    SITEY(g, i) -= lly;
  }

  (void) fprintf(fp, "\\beginpicture\n");
  (void) fprintf(fp,
    "\\setcoordinatesystem units <%lfin,%lfin>\n", units, units);

  /* draw sites */
  for (i=0; i < NS(g); i++) {
    if (a <= SITEA(g, i)  ||  drawDt)
      (void) fprintf(fp, "  \\put {$\\bullet$} at %d %d\n",
        (int) SITEX(g, i), (int) SITEY(g, i));
  }

  /* draw facets */
  if (! drawDt) {
    fprintf(fp, "  \\setshadegrid span <4pt>\n");
    if (a <= g->a0) {
      for (i=0; i < g->nDtcFacets; i++)
        if (a > FACETA(g, i))
          fillFacet(g, i);
    }
    else {
      for (i = g->nDtcFacets; i < NF(g); i++)
        if (a <= FACETA(g, i))
          fillFacet(g, i);
    }
  }

  /* draw edges */
  (void) fprintf(fp, "  \\setlinear\n");

  for (i=0; i < NE(g); i++) {
    if (((EMINA(g, i) < a) && (a <= EMAXA(g, i))) || 
        (drawDt && i < g->nDtcEdges)) {
      qe = MAKEQE(i);
      org = ORG(g, qe);
      dst = DST(g, qe);    
      (void) fprintf(fp, "  \\plot %d %d  %d %d /\n",
        (int) SITEX(g, org), (int) SITEY(g, org),
        (int) SITEX(g, dst), (int) SITEY(g, dst));
    }
  }

  (void) fprintf(fp, "\\endpicture\n");
}  /* -------------------- writePicTeX -------------------- */


main(argc, argv)
int argc;
char **argv;
{
  char *fileName;
  graphType *g = graphNew();

  processArgs(argc, argv, &fileName);
  if (fileName == NULL) {
    usage();
    exit(1);
  }

  readGraph(fileName, g);

  writePicTeX(g);

  gphDispose(g);
}
