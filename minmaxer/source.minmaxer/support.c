/*
 * NAME: support.c
 *
 * DESCRIPTION:
 *	This file contains routines supporting the alpha shapes program
 *	which do not seem to fit in any other category.
 *
 * HISTORY:
 *	89/10/25: drawDt toggle added
 *	89/10/26: This header added
 *	89/10/26: usage() added
 *	89/12/08: alphaAnalyze added
 *	89/12/10: usage() writes to stdout
 */

#include <stdio.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern int GRAPHICON;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef FLOATPOINT
  static char *fmtsitexya = { "Site %3d: %lf %lf %d\n" };
#else
  static char *fmtsitexya = { "Site %3d: %d %d %d\n" };
#endif

extern void drawA();

/* toggles for command line switches */
int graphOn = 1;
int gIncremental = 0;
int showFlips = 0;
int doVerify = 0;
int seqDisplay = 0;
int drawDt = 0;
int alphaAnalyze = 0;


void dumpGraph(g, comment)
graphType *g;
char *comment;
{
  indexType i;

  (void) printf("dumpGraph: %s\n", comment);
  (void) printf("  chEdge %d\n", g->chEdge);
  (void) printf("  nChEdges %d\n", g->nChEdges);
  (void) printf("  nDtcEdges %d\n", g->nDtcEdges);
  (void) printf("  nDtcFacets %d\n", g->nDtcFacets);
  (void) printf("  alphaZero %d\n", g->a0);
  (void) printf("  alpha %d\n", g->alpha);
  (void) printf("  maxs %d, maxe %d, maxf %d, maxa %d\n",
    g->maxs, g->maxe, g->maxf, g->maxa);

  for (i=0; i< NS(g); i++)
    (void) printf(fmtsitexya, i, SITEX(g, i), SITEY(g, i), SITEA(g, i));
  (void) printf("\n");

  /* dump quadedge contents */
  qeDump(stdout, g);

  /* dump facet contents */
  if (NF(g)) {
    for (i=0; i < NF(g); i++)
      (void) printf("facet %d: edges %d %d %d, anum %d\n", i, g->f[i].e[0],
        g->f[i].e[1], g->f[i].e[2], FACETA(g, i));
  }

  /* dump out edge alpha ranges */
  for (i=0; i < NE(g); i++)
    (void) printf("edge %d: amin: %d  amax: %d\n", 
      i, EMINA(g, i), EMAXA(g, i));

  /* dump out alpha values */
  for (i=0; i < NA(g); i++) {
    (void) printf("Alpha value %4d: ", i);

#ifdef FLOATPOINT
    (void) printf("%.0lf\n", g->a[i]);
#else
    if (g->naRational)
      (void) printf("%.0lf/%.0lf = %lf (%lf)\n", 
        lmReal(&g->a[i].num),  lmReal(&g->a[i].den),
        lmReal(&g->a[i].num) / lmReal(&g->a[i].den), g->adbl[i]);
    else
      (void) printf("%lf\n", g->adbl[i]);
#endif
  }
}  /* -------------------- dumpGraph -------------------- */


void fatalError(s)
char *s;
{
  (void) fprintf(stdout, "Fatal error: %s\n", s);
  exit(-1);
}  /* -------------------- fatalError -------------------- */

void usage()

{
  (void) fprintf(stdout, "\nusage: minmaxer inputfile\n");
  /*
    (void) fprintf(stdout, "\nusage: minmaxer [-g -ng] inputfile\n");
    (void) fprintf(stdout, "\t-ng  suppress graphic output (default on sun)\n");
    (void) fprintf(stdout, "\t-g   produce graphic output (default on sgi)\n");\
  */
}  /* ------------------- usage ------------------- */

void processArgs(argc, argv, fileName, banner)
int argc;
char *argv[];
char **fileName;
char * banner;
{
  char *cp;

  *fileName = NULL;

#ifdef sgi
  GRAPHICON = 1;
#else
  GRAPHICON = 0;
#endif

  while (--argc) {
    cp = argv[argc];
    if (*cp != '-')
      *fileName = cp;
    else 
      while (*(++cp)) {
        switch (*cp) {
          case 'n':
	    if (*(cp + 1) == 'g') {
	      printf ("no graphic mode.\n");
	      GRAPHICON = 0;
	      cp++;
	    }
	    else
	      (void) fprintf(stdout, "Bad option \"%c\".\n", *cp);
            break;
          case 'g':
	    GRAPHICON = 1;
            break;
          default:
            (void) fprintf(stdout, "Bad option \"%c\".\n", *cp);
	}
      }
  }

  /* interdependancies of switches */
  if (! graphOn) gIncremental = 0;

  (void) printf(banner);

  if (*fileName == NULL) {
    (void) fprintf(stdout, "MINMAXer: input filename must be given.\n");
    usage();
    exit(1);
  }
  
}  /* -------------------- processArgs -------------------- */


