/*
 * NAME: file_io.c
 *
 * DESCRIPTION:
 *	This file contains routines for reading and writing descriptions
 *	alpha shapes graphs to files.
 *
 * HISTORY:
 *	89/10/26: This header added
 *	89/12/09: readGraph announces number of sites read to stdout
 *	89/12/10: readGraph announcement suppressed
 */

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "road.h"
#include "file_io.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"
#include "triation.h"

#define COMMENTCHAR '#'
#define STRLEN 80
#define ERRFILE stdout

double realAngle(), compAngle();

#ifdef FLOATPOINT
  static char *fmtsitexyzi = { "site %f %f %f %d" };
  static char *fmtsitexyz = { "site %f %f %f" };
  static char *fmtxyz = { "%f %f %f" };
  static char *fmtsitexy = { "site %f %f" };
  static char *fmtxy = { "%f %f" };
#else
  static char *fmtsitexyzi = { "site %d %d %d %d" };
  static char *fmtsitexyz = { "site %d %d %d" };
  static char *fmtxyz = { "%d %d %d" };
  static char *fmtsitexy = { "site %d %d" };
  static char *fmtxy = { "%d %d" };
#endif

extern void dumpGraph();
char input[STRLEN];

static void fileRead(fp, g, countOnly, ns, ne, nf, na)
FILE *fp;
graphType *g;
indexType countOnly;
indexType *ns, *ne, *nf, *na;
{
  char *s;
  coordType x, y, z;
  int s0, s1, e0, e1, e2, amin, amax;
  double adbl;
  int alphaZero, nChEdges, nDtcEdges, nDtcFacets, alpha;
  int dataIs2d, dataIs3d;

  *ns = *ne = *nf = *na = 0;

  dataIs2d = dataIs3d = 0;

  while (fgets(input, STRLEN, fp) != NULL) {
    s = input;
    while(isspace(*s)) s++;		/* skip over leading blanks */
    if (! (*s)) continue;		/* empty line */

    if (*s == COMMENTCHAR) continue;	/* skip comment lines */

    /* attempt to read as a site specification */
    if (sscanf(s, fmtsitexyz, &x, &y, &z) == 3) {
      (*ns)++;
      dataIs3d = 1;
      if (! countOnly) gphLoadSite(g, x, y, z);
    }
    else if (sscanf(s, fmtxyz, &x, &y, &z) == 3) {
      (*ns)++;
      dataIs3d = 1;
      if (! countOnly) gphLoadSite(g, x, y, z);
    }
    else if (sscanf(s, fmtsitexy, &x, &y) == 2) {
      (*ns)++;
      dataIs2d = 1;
      z = 0;
      if (! countOnly) gphLoadSite(g, x, y, z);
    }
    else if (sscanf(s, fmtxy, &x, &y, &z) == 2) {
      dataIs2d = 1;
      (*ns)++;
      z = 0;
      if (! countOnly) gphLoadSite(g, x, y, z);
    }

    /* try to read as an edge specification */
    else if (sscanf(s, "edge %d %d %d %d", &s0, &s1, &amin, &amax) == 4) {
      (*ne)++;
      if (! countOnly) gphLoadEdge(g, s0, s1, amin, amax);
    } 

    /* try to read as a facet specification */
    else if (sscanf(s, "facet %d %d %d %d", &e0, &e1, &e2, &amin) == 4) {
      (*nf)++;
      if (! countOnly) gphLoadFacet(g, e0, e1, e2, amin);
    }

    /* try to read as an alpha value specification */
    else if (sscanf(s, "alpha %lf", &adbl) == 1) {
      (*na)++;
      if (! countOnly) gphLoadAlpha(g, adbl);
    }

    /* try to read as an alpha 0 specification */
    else if (sscanf(s, "alphaZero %d", &alphaZero) == 1) 
      g->a0 = alphaZero;

    else if (sscanf(s, "nChEdges %d", &nChEdges) == 1)
      g->nChEdges = nChEdges;

    else if (sscanf(s, "nDtcEdges %d", &nDtcEdges) == 1)
      g->nDtcEdges = nDtcEdges;

    else if (sscanf(s, "nDtcFacets %d", &nDtcFacets) == 1)
      g->nDtcFacets = nDtcFacets;

    else if (sscanf(s, "draw %d", &alpha) == 1)
      g->alpha = alpha;

    else if (! countOnly) {
      s[strlen(s) - 1] = '\0';		/* get rid of nl */
      (void) fprintf(ERRFILE, "fileRead: bad format: \"%s\".\n", input);
    }
  }

  if (countOnly && dataIs2d && dataIs3d)
    printf ("\nWarning: Input Data: number of coordinates varies\n\n");
  if (countOnly && dataIs2d && (! dataIs3d))
  printf ("\nInput data is two dimensional; z-coordinates initialized to 0.\n");
  if (countOnly && dataIs3d && (! dataIs2d))
    printf ("\nInput data is three dimensional.\n");
}  /* -------------------- fileRead -------------------- */


void readGraph(fname, g)
char *fname;
graphType *g;
{
  FILE *fp, *fopen();
  indexType countOnly, ns, ne, nf, na;

  /* open file pointer */
  if ((fp = fopen(fname, "r")) == NULL) {
    (void) fprintf(ERRFILE, "readGraph:  can't open \"%s\" to read.\n", fname);
    return;
  }

  /* scan file initially only to get counts */
  countOnly = 1;
  fileRead(fp, g, countOnly, &ns, &ne, &nf, &na);
  if (ns == 0) {
    (void) fprintf(ERRFILE, "readGraph: Warning: no sites read in.\n");
    return;
  }

  /* allocate space for parts of graph */
  gphAllocate(g, ns, ne, nf, na);
 
  /* move file pointer back to beginning of file and read in data */
  rewind(fp);
  countOnly = 0;
  fileRead(fp, g, countOnly, &ns, &ne, &nf, &na);

  /* close input file */
  if (fclose(fp) == EOF)
    (void) fprintf(ERRFILE, "readGraph: could not close file \"%s\".\n");


  /* announce the number of sites read */
  /* (void) printf("readGraph: vertices (sites) read: %d\n", ns); */
  /* (void) fflush(stdout); */
}  /* -------------------- readGraph -------------------- */


void writeGraph(fname, g, nofFlips, nofSuccFlips, runTime)
     char *fname;
     graphType *g;
     int nofFlips, nofSuccFlips;
     double runTime;
{
  printf("writing graph");
  char fname2[100];
  int i, j, k, v1, v2, qe, count, edges;
  int triangle[3][3];
  FILE *fp, *fp2;
/*  long clock;*/

  queuesType *angles;
  angleType maxAngle;
  double oldAngle;

  for(i=0; i<strlen(fname)-5; i++) {
    fname2[i] = fname[i];
  }
  fname2[i] = 't';
  fname2[i+1] = 'r';
  fname2[i+2] = 'i';
  fname2[i+3] = '\0';

  if (1) {
    if (! strcmp(fname, "")) {
    fp = stdout;
    fp2 = stdout; 
    } else {
    if ((fp = fopen(fname, "w")) == NULL) { (void)
      fprintf(ERRFILE, "writeGraph: can't open \"%s\" to write.\n", fname);
      return;
    }
    if ((fp2 = fopen(fname2, "w")) == NULL) { (void)
      fprintf(ERRFILE, "writeGraph: can't open \"%s\" to write.\n", fname2);
      return;
    }
    }

/*  (void) fprintf(fp, "cpu time %lf\n", runTime);
  (void) fprintf(fp, "#Flips %d %d\n", nofFlips, nofSuccFlips);
  (void) fprintf(fp, "nChEdges %d\n", g->nChEdges);
  (void) fprintf(fp, "\n");
*/
  /* write out sites (vertices) */
  if (NOS(g)) {
    (void) fprintf(fp, "# site x y z index\n");
    for (i=0; i< NOS(g); i++) {
      (void) fprintf(fp, fmtsitexyzi, SITEX(g, SITEP(g, i)), 
    SITEY(g, SITEP(g, i)), SITEZ(g, SITEP(g, i)), 
    SITEN(g, SITEP(g, i)));
      (void) fprintf(fp, "\n");
    }
    (void) fprintf(fp, "\n");
  }

  /* write out edge information */
  if (NE(g)) {
    fprintf(fp, "# edge orgsite dsts\n"
   );
    for (i=0; i < NE(g); i++) {
      qe = MAKEQE(i);
/*      (void) fprintf(fp, "edge %d %d %d %d %d %d %d %d %d %d %d\n",
    ORG(g, qe), DST(g, qe),
    EMINA(g, i), EMAXA(g, i), i, qe, SYM(qe), ONEXT(g, qe),
    OPREV(g, qe), g->e[qe].locked, g->e[SYM(qe)].locked);

*/
/*      if (ISCHedge (g, i))
fprintf(fp, "000 ");*/
      (void) fprintf(fp, "edge %d %d\n",
    (SITEN (g, ORG(g, qe)) < SITEN (g, DST(g, qe)) ? 
     SITEN (g, ORG(g, qe)) : SITEN (g, DST(g, qe))),
    (SITEN (g, ORG(g, qe)) > SITEN (g, DST(g, qe)) ? 
     SITEN (g, ORG(g, qe)) : SITEN (g, DST(g, qe))));

    }
    (void) fprintf(fp, "\n");
  }


  /* dump out facet information */
  if (NF(g)) {
    (void) fprintf(fp, "# facet e0 e1 e2 alpha\n");
    for (i=0; i < NF(g); i++)
      (void) fprintf(fp, "facet %d %d %d %d\n", g->f[i].e[0],
        g->f[i].e[1], g->f[i].e[2], FACETA(g, i));
    (void) fprintf(fp, "\n");
  }

  /* dump alpha values */
  if (NA(g))
    (void) fprintf(fp, "# alpha value\n");
    for (i=0; i < NA(g); i++)
      (void) fprintf(fp, "alpha %8lf\n", g->adbl[i]);

  /* close output file */
  if (fclose(fp) == EOF) (void)
    fprintf(ERRFILE, "writeGraph: could not close file \"%s\".\n", fname);

  /* write out a time stamp */
  (void) printf("\nTriangulation written to file \"%s\" %s", fname,
    ".\n\n" /* ctime(&clock) */);


  /* write triangle information into fp2*/
  if (NE(g)) {
    count = 0;
    //first loop counts the number of triangles.
    for(k=0; k<2; k++) {
      if( k == 1) {
      //write down number of triangles on first line.
        (void) fprintf(fp2, "%d", count);
        (void) fprintf(fp2, "\n");
      }
      //iterate through every quad edge
      for (i=0; i< NE(g); i++) {
        qe = MAKEQE(i);
        edges = 1;
        //find the vertices of the left triangle of the quad edge
        for(j=0; j<3; j++) {
          triangle[j][0] = SITEX(g, SITEP(g, SITEN(g, ORG(g,qe))));
          triangle[j][1] = SITEY(g, SITEP(g, SITEN(g, ORG(g,qe))));
          triangle[j][2] = SITEZ(g, SITEP(g, SITEN(g, ORG(g,qe))));
          qe = LNEXT(g,qe);
        }
        //check that the vertices form a triangle
        for(j=0; j<3; j++){
          v1 = 0;
          v2 = 1;
          if(j == 1) {
            v2 = 2;
          } else if(j == 2) {
            v1 = 1;
            v2 = 2;
          }
          if(triangle[v1][0] != triangle[v2][0] && triangle[v1][1] !=
            triangle[v2][1] && triangle[v1][1] != triangle[v2][2]) {
            edges = 1;
          } 
        }
        //if we have a triangle and have already counted triangles
        //write down the vertices of the confirmed triangle
        if(k && edges) {
          for(j=0; j<3; j++) {
          (void) fprintf(fp2, fmtxyz, triangle[j][0], triangle[j][1], triangle[j][2]);
          (void) fprintf(fp2, "\n");
          }
        } else {
          //if we are still counting and got a triangle
          if(!k && edges) {
          count = count+1;
          }
        }
      }
    }
  }
  /* close output file2 */
  if (fclose(fp2) == EOF) (void)
    fprintf(ERRFILE, "writeGraph: could not close file \"%s\".\n", fname2);
  }
}  /* -------------------- writeGraph -------------------- */



double realAngle(g, angle)

     graphType *g;
     angleType  angle;

{
  return 0;
}
  
double compAngle(x0, y0, x1, y1, x2, y2)
  
     int x0, x1, x2, y0, y1, y2;

{
  double angle, angle1, angle2;

  if  (x2-x0 !=  0)
    if (y2-y0 != 0)
      angle1 = (double)180./(double)3.1415926 *
	atan((double)(y2-y0)/(double)(x2-x0));
    else
      if (x2-x0 < 0)
	angle1 = 180;
      else
	angle1 = 0;
  else
    angle1 = 90.;

  if (angle1 < 0)
    angle1 += (double)180.;
  if (y2-y0 < 0)
    angle1 += (double)180.;

  if  (x1-x0 !=  0)
    if (y1-y0 != 0)
      angle2 = (double)180./(double)3.1415926 *
	atan((double)(y1-y0)/(double)(x1-x0));
    else
      if (x1-x0 < 0)
	angle2 = 180;
      else
	angle2 = 0;
  else
    angle2 = 90;

  if (angle2 < 0)
    angle2 += (double)180;
  if (y1-y0 < 0)
    angle2 += (double)180;
      
  angle = angle1 - angle2;

  if (angle < 0)
    angle = -angle;

  if (angle > (double)180)
    return (double)360 - angle;
  else
    return angle;
}


fioBuildHeap(g, tree)

     graphType *g;
     queuesType  **tree;
{
  int i, chQe;
  angleType angle;
  *tree = (queuesType *) hpNew(NE(g)*2 - g->nChEdges+2, g);

  for (i = 0; i < NE(g); i++) {
/*    if (! ISCHQE(g, SYM(MAKEQE(i))))
      mmInsertAngle(g, *tree, MAKEQE(i));
    if (! ISCHQE(g, MAKEQE(i)))
      mmInsertAngle(g, *tree, SYM(MAKEQE(i))); */
  }

  chQe = g->chEdge;
  do {
/*    mmQuadEdgeToAngle(g, SYM(ONEXT(g, chQe)), &angle);*/
    hpInsertNode(*tree, angle);
    chQe = ONEXT(g, SYM(chQe));
  } while (chQe != g->chEdge);
}









