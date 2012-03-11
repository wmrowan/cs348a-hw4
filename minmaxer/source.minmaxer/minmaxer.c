/*
 * NAME: road.c
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "road.h"
#include "graph.h"
#include "flips.h"
#include "queue.h"
#include "menu.h"
#include "visual.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int GRAPHICON;
int GLOBAL_INTERRUPT_ALGORITHM;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
initAll (argc, argv, g, visual)

     int argc;
     char *argv[];
     graphType ** g;
     visualType ** visual;

{
  if (GRAPHICON)
    /**visual = stdt2dVisual ();*/
    *visual = dummyVisual ();
  else
    *visual = dummyVisual ();

  *g = newGraph ();

  processArgs(argc, argv, &((*g)->fileName), "\nMINMAXer V1.04 1992\n\n");

  readGraph((*g)->fileName, *g);
  
  if (NS(*g) < 3) fatalError("ERROR: initAll: Must have at least 3 sites");

  /* eliminate duplicate sites */
/*  gphUniqSites(*g);*/
  
  /* if NA(*g) != 0, assume all values for this graph calculated already */
  if (NA(*g)) {
    (void) printf("initAll: input file contains precalculated graph.\n");
    (void) printf("initAll: %d sites, %d edges, %d facets, %d alpha.\n",
		  NS(*g), NE(*g), NF(*g), NA(*g));
    return;
  }
  
  /* allocate space for quadedge edges */
  gphAllocate(*g, 0, (indexType) NS(*g) * 3 - 6, 0, 0);
  
  if (GRAPHICON)
    /* *visual = stdt2dVisual ();*/
    *visual = dummyVisual ();
  else
    *visual = dummyVisual ();

  (*visual)->menu  = (*(*visual)->makeMainMenu) ((*visual), (*g));     /* :-) */

  if (GRAPHICON)
    sprintf ((*visual)->winTitle, "MINMAXer  v 1.04");

  (*(*visual)->initialize) ((*g), (*visual));
}

/*---------------------------------------------------------------------------*/

main(argc,argv)

     int argc;
     char *argv[];

{
  int nofFlips, nofAttempts, button;
  double runTime;
  graphType * g;
  visualType * visual;
  char * fout;

  initAll(argc, argv, &g, &visual);

  DONT_INTERRUPT_ALGORITHM;

  do {
    if (SHOULD_INTERRUPT_ALGORITHM)
      printf ("\n Algorithm Interrupted. \n\n");
    (*visual->resetChoice) (visual->menu, visual, g);
    button = (*visual->getChoiceButton) (visual->menu, visual, g);
    DONT_INTERRUPT_ALGORITHM;

    switch (button) {
    case 0:
      break;
    case 1:
      (*visual->pause) (g, visual);
      buildInitialTriangulation (g, visual, &runTime);
      writeGraph("initial.edges", g, 0, 0, (double) 0);
      break;
    case 2:
      (*visual->pause) (g, visual);
      buildDelaunayTriangulation 
	(g, visual, &nofFlips, &nofAttempts, &runTime);
      writeGraph("delaunay.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 3:
      (*visual->pause) (g, visual);
      buildMinMaxAngleTriangulation (g, visual, &nofFlips,
				     &nofAttempts, &runTime);  
      writeGraph("minmax.angle.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 4:
      (*visual->pause) (g, visual);
      buildMinMaxAngleTriangulation2 (g, visual, &nofFlips,
				      &nofAttempts, &runTime); 
      writeGraph("minmax2.angle.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 5:
      (*visual->pause) (g, visual);
      buildMaxMinHeightTriangulation (g, visual, &nofFlips,
				      &nofAttempts, &runTime); 
      writeGraph("maxmin.height.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 6:
      (*visual->pause) (g, visual);
      buildMaxMinHeightTriangulation2 (g, visual, &nofFlips,
				      &nofAttempts, &runTime); 
      writeGraph("maxmin.height.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 7:
      (*visual->pause) (g, visual);
      buildMinMaxSlopeTriangulation (g, visual, &nofFlips,
				      &nofAttempts, &runTime); 
      writeGraph("minmax.slope.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 8:
      (*visual->pause) (g, visual);
      (*visual->switchToPQE) (visual);
      newDelaunayTriangulation 
	(g, visual, &nofFlips, &nofAttempts, &runTime);
      writeGraph("delaunay.incremental.edges", 
		 g, nofFlips, nofAttempts, runTime);
      break;
    case 9:
      (*visual->pause) (g, visual);
      (*visual->switchToPQE) (visual);
      regularTriangulation 
	(g, visual, &nofFlips, &nofAttempts, &runTime);
      writeGraph("regular.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 10:
      verifyRegular (g, 1);
      break;
    case 11:
      verifyRegular (g, 0);
      writeGraph("regular.edges", g, nofFlips, nofAttempts, runTime);
      break;
    case 12:
      quickSortSite(g->s, NS(g), LTsite, swapSite);
      gphUniqSites(g, swapSite);
      break;
    case 13:
      fout = (*visual->getFileName) (visual, g);
      if (fout[0] != '\0')
	writeGraph(fout, g, nofFlips, nofAttempts, runTime);
      break;
    }
    if ((button != 0) && (button != 10) && (button != 11) && (button != 12) &&
	(button != 13))
      printGraphInfo (g);

  } while (button != 0);

  (*visual->cleanUp) (g, visual);

  return 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


