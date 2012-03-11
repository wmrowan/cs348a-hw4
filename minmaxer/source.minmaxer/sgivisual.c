/*
 * NAME: visual.c
 *
 * DESCRIPTION:
 *	contains visualization functions such as drawing and erasing
 *      edges ...
 *
 */

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <gl.h>
#include <device.h>
#include <limits.h>
#include <malloc.h>
#include "road.h"
#include "graph.h"
#include "sgivislib.h"
#include "menu.h"
#include "visual.h"
#include "trackball.h"

/*---------------------------------------------------------------------------*/

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int
dummyFun ()

{}


void 
dummyFun2 ()

{
}

int
returnFalse (dummy)

     double dummy;

{
  return 0;
}

bool
stdt2dDummyGT ()

{
  return 1;
}

float
stdt2dDummyRat ()


{
  return 1.0;
}

/*---------------------------------------------------------------------------*/

printMatrix (m)

     Matrix m;

{
  int i, j;

  for (i = 0; i <= 3; i++) {
    for (j = 0; j <= 3; j++)
      printf ("%f ",m[i][j]);
    printf ("\n");
  }
    printf ("\n");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static float yellow_material[] =
{
  ALPHA, 1.0,                       /* ignored */
  AMBIENT, 0.5, 0.5, 0.5,
  COLORINDEXES, 0.0, 127.5, 225.0,  /* ignored */
  DIFFUSE, 0.0, 0.0, 1.0,                            /* YELLOW */
  EMISSION, 0.0, 0.0, 0.0,          /* ignored */
  SHININESS, 1.0, 
  SPECULAR, 0.3, 0.3, 0.3,
  LMNULL,
};

static float the_light_model[] =
{
  AMBIENT, 0.1, 0.1, 0.1,
  LOCALVIEWER, 0.0,
  ATTENUATION, 0.1, 1.0,
  LMNULL,
};

float white_light[] =
{
  LCOLOR, 1, 1, 1,
  POSITION, 0, 0, 1, 0,
  LMNULL,
};

float white_light_opposite[] =
{
  LCOLOR, 1, 1, 1,
  POSITION, 0, 0, -1, 0,
  LMNULL,
};

float standard_light_model[] =
{
  AMBIENT, .1, .1, .1,
  LOCALVIEWER, 0,
  ATTENUATION, 1.0, 0.0,
  LMNULL,
};

float singular_material[] =
{
  DIFFUSE, .0, .8, 0,   
  AMBIENT, .5, .5, .5,
  SPECULAR, .5, .5, .5,
  LMNULL,
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
makeViewing (actVisual)

     visualType *actVisual;

{
  Matrix tmpM;

  build_rotmatrix (tmpM, actVisual->rvec);

  MMODE (MPROJECTION);
  LOADMATRIX (ident);
  MMODE (MVIEWING);
  LOADMATRIX (ident);

  PERSPECTIVE
    (actVisual->ang, 1.0,
     MAX (0.00000001,
          actVisual->scale * (actVisual->COPz - actVisual->zMean - 1.75 * actVisual->maxDiff - 1)),
     actVisual->scale * actVisual->COPz - actVisual->zMean + 10 * actVisual->maxDiff + 1);

  LOADMATRIX (ident);
  LOOKAT (actVisual->COPx, actVisual->COPy, actVisual->scale * actVisual->COPz,
	  actVisual->COPx, actVisual->COPy, actVisual->zMean, 0);
  TRANSLATE ((actVisual->x0 + actVisual->x1) / 2.0,
	     (actVisual->y0 + actVisual->y1) / 2.0,
	     actVisual->zMean);
  MULTMATRIX (tmpM);
  TRANSLATE ((- actVisual->x0 - actVisual->x1) / 2.0,
	     (- actVisual->y0 - actVisual->y1) / 2.0,
	     -actVisual->zMean);
}

/*---------------------------------------------------------------------------*/

void
rotateToInitialPosition (actVisual)

     visualType * actVisual;

{
  actVisual->rvec[0] = actVisual->rvec[1] = actVisual->rvec[2] = 0.0;
  actVisual->rvec[3] = 1.0;
  actVisual->trans[0] = actVisual->trans[1] = actVisual->trans[2] = 0.0;
  actVisual->DeltaWOld = actVisual->DeltaHOld = actVisual->DeltaW =
  actVisual->DeltaH = 0;

  makeViewing (actVisual);
}

/*---------------------------------------------------------------------------*/

void
initZcoord (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
  actVisual->zMean = (g->zMax - g->zMin) / 2.0;
  
  actVisual->COPx = (actVisual->x1 + actVisual->x0) / 2.0;
  actVisual->COPy = (actVisual->y1 + actVisual->y0) / 2.0;
  if (MAX (g->zMax, -g->zMin) != 0)
    actVisual->COPz = 10.0 * MAX (g->zMax, -g->zMin);
  else
    actVisual->COPz = 3.0 * 
      MAX (g->xMax, MAX (-g->xMin, MAX (g->yMax,-g->yMin)));

  actVisual->maxDiff = (g->xMax - g->xMin) / 2.0;
  actVisual->maxDiff = MAX (actVisual->maxDiff, (g->yMax - g->yMin) / 2.0);
  actVisual->ang = 
    MAX (2.0, 
	 10.0 * atan (actVisual->maxDiff / actVisual->COPz) * 230.0);
  actVisual->maxDiff = MAX (actVisual->maxDiff, (g->zMax - g->zMin) / 2.0);

  makeViewing (actVisual);
}

/*---------------------------------------------------------------------------*/

void
initX2Y2coord (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
  actVisual->zMean = 
    ((double) (g->yMax*g->yMax + g->xMax * g->xMax) +
     (double) (g->yMin*g->yMin + g->xMin * g->xMin)) / 2.0 /
       MAX (g->xMax - g->xMin, g->yMax - g->yMin);

  actVisual->COPx = (actVisual->x1 + actVisual->x0) / 2.0;
  actVisual->COPy = (actVisual->y1 + actVisual->y0) / 2.0;
  if (MAX (g->xMax - g->xMin, g->yMax - g->yMin) != 0)
    actVisual->COPz = 
      20.0 * (double) (g->yMax*g->yMax + g->xMax * g->xMax) /
	MAX (g->xMax - g->xMin, g->yMax - g->yMin);
  else
    actVisual->COPz = 3.0 * 
      MAX (g->xMax, MAX (-g->xMin, MAX (g->yMax,-g->yMin)));
  
  actVisual->maxDiff = (g->xMax - g->xMin) / 2.0;
  actVisual->maxDiff = MAX (actVisual->maxDiff, (g->yMax - g->yMin) / 2.0);
  actVisual->ang = 
    MAX (2.0, 10.0 * atan (actVisual->maxDiff / actVisual->COPz) * 230.0);
  actVisual->maxDiff = 
    MAX (actVisual->maxDiff, 
	 ((double) (g->yMax*g->yMax + g->xMax * g->xMax) -
	  (double) (g->yMin*g->yMin + g->xMin * g->xMin)) / 2.0 /
	 MAX (g->xMax - g->xMin, g->yMax - g->yMin));

  makeViewing (actVisual);
}

/*---------------------------------------------------------------------------*/

void
initX2Y2Zcoord (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
  indexType vertex;
  double tmp, tmpMax;
  double tmpL;

  tmp = 0;
  tmpMax = 0;

  for (vertex = 0; vertex < NS(g); vertex++) {
    tmpL = getX2Y2minusZ (g, vertex);
    tmp += tmpL;
    tmpMax = MAX (tmpMax, tmpL);
  }

  actVisual->zMean = tmp / (double) NS (g);

  actVisual->COPx = (actVisual->x1 + actVisual->x0) / 2.0;
  actVisual->COPy = (actVisual->y1 + actVisual->y0) / 2.0;
  if (MAX (g->xMax - g->xMin, g->yMax - g->yMin) != 0)
    actVisual->COPz = 10.0 * tmpMax;
  else
    actVisual->COPz = 3.0 * 
      MAX (g->xMax, MAX (-g->xMin, MAX (g->yMax,-g->yMin)));
  
  actVisual->maxDiff = (g->xMax - g->xMin) / 2.0;
  actVisual->maxDiff = MAX (actVisual->maxDiff, (g->yMax - g->yMin) / 2.0);
  actVisual->ang = 
    MAX (2.0, 10.0 * atan (actVisual->maxDiff / actVisual->COPz) * 230.0);
  actVisual->maxDiff = 
    MAX (actVisual->maxDiff, 
	 ((double) (g->yMax*g->yMax + g->xMax * g->xMax) -
	  (double) (g->yMin*g->yMin + g->xMin * g->xMin)) / 2.0 /
	 MAX (g->xMax - g->xMin, g->yMax - g->yMin));

  makeViewing (actVisual);
}

/*---------------------------------------------------------------------------*/

void
stdt2dInitialize (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
  int site, i, j;
  float xRatio, yRatio;
  int offs, xOffs, yOffs, x0, x1, y0, y1;

  getColor = stdt2dDummyRat;

  actVisual->window = (long) corner2dWindow (actVisual->winTitle, 
					     actVisual->xLen, actVisual->yLen, 
					     actVisual->colorTable[0]);

  DOUBLEBUFFER ();
  GCONFIG ();

  actVisual->scale = (double) (g->yMax - g->yMin) / 
                     (double) (g->xMax - g->xMin);

  if (actVisual->scale < 1.0) {
    offs = (actVisual->scale - 1.0) / 2.0 * (float) (g->xMax - g->xMin);
    x0 = g->xMin - offs;
    x1 = g->xMax + offs;
    y0 = g->yMin;
    y1 = g->yMax;
  } else {
    offs = (actVisual->scale - 1.0) / 2.0 * (float) (g->yMax - g->yMin);
    x0 = g->xMin;
    x1 = g->xMax;
    y0 = g->yMin - offs;
    y1 = g->yMax + offs;
  }

  actVisual->x0 = x0;
  actVisual->y0 = y0;
  actVisual->x1 = x1;
  actVisual->y1 = y1;

  ZBUFFER (FALSE);

  actVisual->scale = 1.0;
  initZcoord (g, actVisual);
  rotateToInitialPosition (actVisual);

  LMDEF (DEFMATERIAL, 2, 0, singular_material);
  LMDEF (DEFMATERIAL, 1, 0, yellow_material);
  LMDEF (DEFLMODEL, 1, 0, standard_light_model);
  LMDEF (DEFLIGHT, 1, 0, white_light);
  LMDEF (DEFLIGHT, 2, 0, white_light_opposite);

  LMBIND (MATERIAL, 1);
  LMBIND (LMODEL, 1);
  LMBIND (LIGHT0, 1);
  LMBIND (LIGHT1, 2);
  NMODE (NNORMALIZE);

  (*actVisual->redrawWindow) (g, actVisual);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dDrawEdge (g, actVisual, qe, cCode)
     
     graphType * g;
     indexType qe;
     visualType * actVisual;
     int       cCode;

{
  (*actVisual->unsetDeleted) (g, (*actVisual->qetoe) (qe));
  (*actVisual->redrawEdge) (g, actVisual, qe, cCode);
  (void) actVisual->getChoiceButton (actVisual->menu, actVisual, g);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dNoDraw (g, actVisual, qe, cCode)
     
     graphType * g;
     indexType qe;
     visualType * actVisual;
     int       cCode;

{
  (void) actVisual->getChoiceButton (actVisual->menu, actVisual, g);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dDrawNiceEdge (g, actVisual, qe, cCode)
     
     graphType * g;
     indexType qe;
     visualType * actVisual;
     int       cCode;

{
  (*actVisual->unsetDeleted) (g, (*actVisual->qetoe) (qe));
  (actVisual->redrawWindow) (g, actVisual);
  (void) actVisual->getChoiceButton (actVisual->menu, actVisual, g);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dEraseNiceEdge (g, actVisual, qe, cCode)
     
     graphType * g;
     indexType qe;
     visualType * actVisual;
     int       cCode;

{
  (*actVisual->setDeleted) (g, (*actVisual->qetoe) (qe));
  (*actVisual->redrawWindow) (g, actVisual);
  (void) actVisual->getChoiceButton (actVisual->menu, actVisual, g);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dEraseEdge (g, actVisual, qe)
     
     graphType * g;
     indexType qe;
     visualType * actVisual;

{
  (*actVisual->redrawEdge) (g, actVisual, qe, 0);
  (*actVisual->setDeleted) (g, (*actVisual->qetoe) (qe));
  (void) actVisual->getChoiceButton (actVisual->menu, actVisual, g);
}

/*---------------------------------------------------------------------------*/

void
stdt2dCleanUp (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
}

/*---------------------------------------------------------------------------*/

void
stdt2dPause (g, actVisual)

     graphType * g;
     visualType * actVisual;

{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int 
outOfTime (Delay)

     double Delay;

{
  static int firstTime = 1;
  static double startTime;

  if (firstTime) {
    startTime = get_time_of_day ();
    firstTime = 0;
  }

  if ((get_time_of_day () - startTime) >= Delay) {
    startTime = get_time_of_day ();
    return 1;
  } else
    return 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
scaleView (actVisual, g)

     visualType * actVisual;
     graphType *g;

{
  int h, w, hOld, wOld;
  long saveWindow;
  long orgX, orgY;
  long sizeX, sizeY;
  short buttonMode;
  long dev;
  Matrix tmpM, tM;
  float tmpV[4];

  saveWindow = WINGET ();
  WINSET (actVisual->window);

  GETORIGIN (&orgX, &orgY);
  GETSIZE (&sizeX, &sizeY);

  buttonMode =  GETBUTTON (MIDDLEMOUSE);
  hOld = GETVALUATOR (MOUSEY) - orgY;
  wOld = GETVALUATOR (MOUSEX) - orgX;

  if ((wOld > sizeX) || (hOld > sizeY) || (! buttonMode)) {
    WINSET (saveWindow);
    return;
  }

  (*actVisual->rotateRedraw) (g, actVisual);

  while (buttonMode == TRUE) {

    h = GETVALUATOR (MOUSEY) - orgY;
    w = GETVALUATOR (MOUSEX) - orgX;

    if ((w != wOld) || (h != hOld)) {

      actVisual->scale = actVisual->scale *
        (-1.00001 * ((hOld - h) / (float) sizeY * 2.0 - 1.0));

      makeViewing (actVisual);

      (*actVisual->rotateRedraw) (g, actVisual);
    }

    hOld = h;
    wOld = w;
    buttonMode =  GETBUTTON (MIDDLEMOUSE);
  }

  (*actVisual->redrawWindow) (g, actVisual);

  WINSET (saveWindow);
}


/*---------------------------------------------------------------------------*/

void
rotateView (actVisual, g)

     visualType * actVisual;
     graphType * g;

{
  int h, w, hOld, wOld;
  long saveWindow;
  long orgX, orgY;
  long sizeX, sizeY;
  short buttonMode;
  long dev;
  Matrix tmpM;
  float tmpV[4];

  if (GETBUTTON (MIDDLEMOUSE)) {
    scaleView (actVisual, g);
    return;
  }

  saveWindow = WINGET ();
  WINSET (actVisual->window);
  
  GETORIGIN (&orgX, &orgY);
  GETSIZE (&sizeX, &sizeY);
  
  if (GETBUTTON (RIGHTMOUSE)) {
    rotateToInitialPosition (actVisual);
    (*actVisual->redrawWindow) (g, actVisual);
    WINSET (saveWindow);
    return;
  }
    
  buttonMode =  GETBUTTON (LEFTMOUSE);
  hOld = GETVALUATOR (MOUSEY) - orgY;
  wOld = GETVALUATOR (MOUSEX) - orgX;

  if ((wOld > sizeX) || (hOld > sizeY) || (! buttonMode)) {
    WINSET (saveWindow);
    return;
  }

  (*actVisual->rotateRedraw) (g, actVisual);

  while (buttonMode == TRUE) {
    
    h = GETVALUATOR (MOUSEY) - orgY;
    w = GETVALUATOR (MOUSEX) - orgX;

    if ((w != wOld) || (h != hOld)) {

      trackball (tmpV, (float) 2.0 * (wOld - orgX) / (float) sizeX - 1.0,
		 (float) 2.0 * (hOld - orgY) / (float) sizeY - 1.0,
		 (float) (2.0 * (w - orgX) / (float) sizeX - 1.0),
		 (float) (2.0 * (h - orgY) / (float) sizeY - 1.0));
      add_quats (tmpV, actVisual->rvec, actVisual->rvec);
      
      makeViewing (actVisual);
      
      (*actVisual->rotateRedraw) (g, actVisual);
      
      actVisual->DeltaWOld = 2.0 * (wOld - orgX) / (float) sizeX - 1.0;
      actVisual->DeltaHOld = 2.0 * (hOld - orgY) / (float) sizeY - 1.0;
      actVisual->DeltaW = 2.0 * (w - orgX) / (float) sizeX - 1.0;
	actVisual->DeltaH = 2.0 * (h - orgY) / (float) sizeY - 1.0;
    } else {
      actVisual->DeltaWOld = actVisual->DeltaHOld = actVisual->DeltaW = 
      actVisual->DeltaH = 0;
    }

    hOld = h;
    wOld = w;
    buttonMode =  GETBUTTON (LEFTMOUSE);
  }

  (*actVisual->redrawWindow) (g, actVisual);
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void
autoRotateView (actVisual, g)

     visualType * actVisual;
     graphType * g;

{
  long saveWindow;
  long dev;
  Matrix tmpM;
  float tmpV[4];

  if ((actVisual->DeltaWOld != actVisual->DeltaW) ||
       (actVisual->DeltaHOld != actVisual->DeltaH)) {

    trackball (tmpV, actVisual->DeltaWOld, actVisual->DeltaHOld,
	       actVisual->DeltaW,  actVisual->DeltaH);
    add_quats (tmpV, actVisual->rvec, actVisual->rvec);

    makeViewing (actVisual);    

    (*actVisual->rotateRedraw) (g, actVisual);

  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

long
getX2Y2 (g, vertex)

     graphType *g;
     indexType vertex;

{
  return (long) (SITEX (g, vertex) * SITEX (g, vertex) + 
		 SITEY (g, vertex) * SITEY (g, vertex)) / 
		   MAX (g->xMax - g->xMin, g->yMax - g->yMin);
}

/*---------------------------------------------------------------------------*/

long
getX2Y2minusZ (g, vertex)

     graphType *g;
     indexType vertex;

{
  return (long) (SITEX (g, vertex) * SITEX (g, vertex) + 
		 SITEY (g, vertex) * SITEY (g, vertex) - 
		 SITEZ (g, vertex)) /
		   MAX (g->xMax - g->xMin, g->yMax - g->yMin);
}

/*---------------------------------------------------------------------------*/

long
getZ (g, vertex)

     graphType *g;
     indexType vertex;

{
  return SITEZ(g, vertex);
}

/*---------------------------------------------------------------------------*/

long
get0 (g, vertex)

     graphType *g;
     indexType vertex;

{
  return (long) (g->zMax + g->zMin) / 2;
}

/*---------------------------------------------------------------------------*/

static indexType
visualPOnext (g, qe)

     graphType *g;
     indexType qe;

{
  return pONEXT (g, qe, TIME_NOW);
}

/*---------------------------------------------------------------------------*/

static indexType
visualPOprev (g, qe)

     graphType *g;
     indexType qe;

{
  return pOPREV (g, qe, TIME_NOW);
}

/*---------------------------------------------------------------------------*/

void
assignQEfunctions (visual)

     visualType * visual;

{
  visual->org = ORG;
  visual->dst = DST;
  visual->sym = SYM;
  visual->onext = ONEXT;
  visual->oprev = OPREV;
  visual->qetoe = QETOE;
  visual->makeqe = MAKEQE;
  visual->isDeleted = ISDELETEDedge;
  visual->setDeleted = SETDELETEDedge;
  visual->unsetDeleted = UNSETDELETEDedge;
}

/*---------------------------------------------------------------------------*/

static int
pEdgeNotDrawAble (g, edge)

     graphType * g;
     indexType edge;

{
  indexType qe;
  
  if (g->pe == NULL)
    return 0;
  qe = pMAKEQE (edge);
  return (! ((pORG (g, qe) >= 0) && (pDST (g, qe) >= 0) &&
         (pqeExistsAtTime (g, qe, TIME_NOW))));

}

/*---------------------------------------------------------------------------*/

void
assignPQEfunctions (visual)

     visualType * visual;

{
  visual->org = pORG;
  visual->dst = pDST;
  visual->sym = pSYM;
  visual->onext = visualPOnext;
  visual->oprev = visualPOprev;
  visual->qetoe = pQETOE;
  visual->makeqe = pMAKEQE;
  visual->isDeleted = pEdgeNotDrawAble;
  visual->setDeleted = dummyFun2;
  visual->unsetDeleted = dummyFun2;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* define an instance "stdt2dVisual" using the stdt2d visual functions */

visualType *
stdt2dVisual ()

{
  visualType * tmp;

  if ((tmp = (visualType *) malloc (sizeof (visualType))) == NULL) {
    (void) fprintf (stdout, "makeStdt2dVisual: fail on [mc]alloc.\n");
    exit (1);
  }
 
  tmp->initialize = stdt2dInitialize;
  tmp->drawEdge = stdt2dDrawNiceEdge;
  tmp->redrawEdge = stdt2dRedrawEdge;
  tmp->eraseEdge = stdt2dEraseNiceEdge;
  tmp->cleanUp = stdt2dCleanUp;   
  tmp->redrawWindow = wireframe2dRedrawWindow;
  tmp->makeMainMenu = makeMainMenu;
  tmp->getChoiceButton = noAlgorithmLoop;
  tmp->getFileName = getFileNameLoop;
  tmp->resetChoice = resetChoice;
  tmp->pause = stdt2dPause;
  tmp->quality2dSetFunctions = quality2dSetFunctions;
  tmp->rotateRedraw = wireframe2dRedrawWindow;
  tmp->autoRotateView = dummyFun2;
  tmp->drawVertices = drawVerticesTetrahedra;
  tmp->getThirdCoord = getZ;
  tmp->switchToPQE = assignPQEfunctions;
  assignQEfunctions (tmp);
  tmp->winTitle = "Standart 2 dimensional Window"; 
  tmp->xLen = 983; 
  tmp->yLen = 983; 
  tmp->scale = 0.9; 
  tmp->colorTable [0][0] = 0;
  tmp->colorTable [0][1] = 0;
  tmp->colorTable [0][2] = 0;
  tmp->colorTable [1][0] = 0;
  tmp->colorTable [1][1] = 0.8;
  tmp->colorTable [1][2] = 0;
  tmp->colorTable [2][0] = 0;
  tmp->colorTable [2][1] = 0.4;
  tmp->colorTable [2][2] = 1;
  tmp->colorTable [3][0] = 1;
  tmp->colorTable [3][1] = 1;
  tmp->colorTable [3][2] = 1;
  tmp->timeSup = outOfTime;
  tmp->delayTime = 1.0;
  return tmp;
}

/*---------------------------------------------------------------------------*/

void
doBigWindowAction (g, actVisual)

     visualType * actVisual;
     graphType * g;

{
  int dev;
  short value;

  dev = QREAD (&value);

  if (dev == REDRAW) 
    (*actVisual->redrawWindow) (g, actVisual);
  else 
    rotateView (actVisual, g);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/







