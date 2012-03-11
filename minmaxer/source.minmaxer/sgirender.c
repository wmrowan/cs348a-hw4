/*
 * NAME: render.c
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

/*---------------------------------------------------------------------------*/

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*---------------------------------------------------------------------------*/

Matrix ident =
{
	{ 1.0, 0.0, 0.0, 0.0,},
	{ 0.0, 1.0, 0.0, 0.0,},
	{ 0.0, 0.0, 1.0, 0.0,},
	{ 0.0, 0.0, 0.0, 1.0,},
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

float
getColorMaxAngle (g, visual, edge1)

     graphType * g;
     visualType *visual;
     indexType edge1;

{
  return 
    (sosAngleFloat (
        (*visual->org) (g, edge1), SITEX(g,  (*visual->org) (g, edge1)), SITEY(g, (*visual->org) (g, edge1)),
		       SITEZ (g, (*visual->org) (g, edge1)),
        (*visual->dst) (g, (*visual->onext) (g, edge1)), SITEX(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
		    SITEY(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
		    SITEZ (g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
        (*visual->dst) (g, edge1), SITEX(g, (*visual->dst) (g, edge1)), SITEY(g,(*visual->dst) (g, edge1)),
		       SITEZ (g,(*visual->dst) (g, edge1)))
     - 60.0 ) / 120.0;
}

/*---------------------------------------------------------------------------*/

float
getColorMinAngle (g, visual, edge1)

     graphType * g;
     visualType *visual;
     indexType edge1;

{

  return  - 2.0 * getColorMaxAngle (g, visual, edge1);
}

/*---------------------------------------------------------------------------*/

float
getColorMinHeight (g, visual, edge1)

     graphType * g;
     visualType *visual;
     indexType edge1;

{
  static float tmp;

  tmp = sosHeightFloat 
    ((*visual->org) (g, edge1), 
     SITEX(g,  (*visual->org) (g, edge1)), SITEY(g, (*visual->org) (g, edge1)),
     SITEZ (g, (*visual->org) (g, edge1)),
     (*visual->dst) (g, (*visual->onext) (g, edge1)), 
     SITEX(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
     SITEY(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
     SITEZ (g, 
			      (*visual->dst) (g, (*visual->onext) (g, edge1))),
     (*visual->dst) (g, edge1), 
     SITEX(g, (*visual->dst) (g, edge1)), SITEY(g,(*visual->dst) (g, edge1)),
     SITEZ (g,(*visual->dst) (g, edge1)));

  return 1.0 - tmp / MAX (g->xMax - g->xMin, g->yMax - g->yMin) * 4.0;
}

/*---------------------------------------------------------------------------*/

float
getColorMaxSlope (g, visual, edge1)

     graphType * g;
     visualType *visual;
     indexType edge1;

{
  return 
    sosSlopeFloat (
        (*visual->org) (g, edge1), SITEX(g,  (*visual->org) (g, edge1)), SITEY(g, (*visual->org) (g, edge1)),
		       SITEZ (g, (*visual->org) (g, edge1)),
        (*visual->dst) (g, (*visual->onext) (g, edge1)), SITEX(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
		    SITEY(g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
		    SITEZ (g, (*visual->dst) (g, (*visual->onext) (g, edge1))),
        (*visual->dst) (g, edge1), SITEX(g, (*visual->dst) (g, edge1)), SITEY(g,(*visual->dst) (g, edge1)),
		       SITEZ (g,(*visual->dst) (g, edge1))) 
     / 90.0;
}

/*---------------------------------------------------------------------------*/

void
quality2dSetFunctions (g, visual, colorFun)

     graphType * g;
     visualType * visual;
     float (*colorFun) ();

{
  getColor = colorFun;

  (*visual->redrawWindow) (g, visual);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void
crossProduct(g, visual, v1, v2, v3, cross)

     graphType *g;
     visualType *visual;
     indexType v1, v2, v3;
     float cross[3];

{
  double dx1, dy1, dz1, dx2, dy2, dz2, length;

  dx1 = SITEX (g, v2) - SITEX (g, v1);
  dy1 = SITEY (g, v2) - SITEY (g, v1);
  dz1 = (*visual->getThirdCoord) (g, v2) - (*visual->getThirdCoord) (g, v1);

  dx2 = SITEX (g, v3) - SITEX (g, v2);
  dy2 = SITEY (g, v3) - SITEY (g, v2);
  dz2 = (*visual->getThirdCoord) (g, v3) - (*visual->getThirdCoord) (g, v2);

  cross[0] = (dy1 * dz2) - (dz1 * dy2);
  cross[1] = (dz1 * dx2) - (dx1 * dz2);
  cross[2] = (dx1 * dy2) - (dy1 * dx2);

  length = sqrt ((double) cross[0] * (double) cross[0] + 
		 (double) cross[1] * (double) cross[1] +
		 (double) cross[2] * (double) cross[2]);

  cross[0] = cross[0] / length;
  cross[1] = cross[1] / length;
  cross[2] = cross[2] / length;
}

/*---------------------------------------------------------------------------*/

static
makeColor (ratio, color)

     float ratio;
     colorType color;

{
  color [0] = (00 + (1.0 - fsin (ratio * 1.5707963)) * 200.0) / 256.0;
  color [1] = (20 + (1.0 - fsin (ratio * 1.5707963)) * 200.0) / 256.0;
  color [2] = (40+ (1.0 - fsin (ratio * 1.5707963)) * 200.0) / 256.0;
}

/*---------------------------------------------------------------------------*/

void 
quality2dEraseEdge (g, visual, qe)
     
     graphType * g;
     indexType qe;
     visualType * visual;

{
  (*visual->redrawEdge) (g, visual, qe, NULL);
  SETDELETEDedge (g, (*visual->qetoe) (qe));
  (void) visual->getChoiceButton (visual->menu, visual, g);
}

/*---------------------------------------------------------------------------*/

void 
showQualityBar (g, visual)
     
     graphType * g;
     visualType * visual;

{
  colorType actColor;
  float tmp1[3], tmp2[3], tmp3[3], tmp4[3];
  Matrix v, p;

  MMODE (MPROJECTION);
  GETMATRIX (p);
  LOADMATRIX (ident);
  MMODE (MVIEWING);
  GETMATRIX (v);
  LOADMATRIX (ident);

  tmp1[0] = 0;
  tmp1[1] = 0;
  tmp2[0] = 0;
  tmp2[1] = 800;
  tmp3[0] = 800;
  tmp3[1] = 800;
  tmp4[0] = 800;
  tmp4[1] = 0;

  BGNPOLYGON (); {
    makeColor ((float) 0.0, actColor);
    C3F (actColor);
    V2S (tmp1);
    V2S (tmp2);
    makeColor ((float) 1.0, actColor);
    C3F (actColor);
    V2S (tmp3);
    V2S (tmp4);
  } ENDPOLYGON ();

  MMODE (MPROJECTION);
  LOADMATRIX (p);
  MMODE (MVIEWING);
  LOADMATRIX (v);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void
drawTinyTetrahedron (g, visual, vertex, offs, offs2)

     graphType * g;
     visualType * visual;
     indexType vertex;
     long offs, offs2;

{
  long tmp[3];

  BGNTMESH (); {
    tmp[0] = SITEX (g, vertex) - offs;
    tmp[1] = SITEY (g, vertex) - offs;
    tmp[2] = (*visual->getThirdCoord) (g, vertex) - offs;
    V3I (tmp);
    tmp[0] += offs2;
    V3I (tmp);
    tmp[1] += offs2;
    V3I (tmp);
    tmp[2] += offs2;
    V3I (tmp);
    tmp[0] = SITEX (g, vertex) - offs;
    tmp[1] = SITEY (g, vertex) - offs;
    tmp[2] = (*visual->getThirdCoord) (g, vertex) - offs;
    V3I (tmp);
  } ENDTMESH ();
}
  
/*---------------------------------------------------------------------------*/

void
drawVerticesTetrahedra (g, visual)

     graphType * g;
     visualType * visual;

{
  long offs, offs2;
  float ftmp[3];
  indexType vertex;

  ftmp[0] = 0; ftmp[1] = 1; ftmp[2] = 1;
  C3F (ftmp);
  
  offs = MIN (g->xMax - g->xMin, g->yMax - g->yMin) / 350.0;
  offs2 = MIN (g->xMax - g->xMin, g->yMax - g->yMin) / 175.0;
  
  for (vertex = 0; vertex < NS(g); vertex++) {
    drawTinyTetrahedron (g, visual, vertex, offs, offs2);
  }
}

/*---------------------------------------------------------------------------*/

static void
doDrawTriangle (g, visual, org, dst, dst2)

     graphType * g;
     visualType * visual;
     indexType org, dst, dst2;

{
  long tmp[3];

  BGNPOLYGON (); {
    tmp[0] = SITEX (g, org);
    tmp[1] = SITEY (g, org);
    tmp[2] = (*visual->getThirdCoord) (g, org);
    V3I (tmp);
    tmp[0] = SITEX (g, dst);
    tmp[1] = SITEY (g, dst);
    tmp[2] = (*visual->getThirdCoord) (g, dst);
    V3I (tmp);
    tmp[0] = SITEX (g, dst2);
    tmp[1] = SITEY (g, dst2);
    tmp[2] = (*visual->getThirdCoord) (g, dst2);
    V3I (tmp);
  } ENDPOLYGON ();
}	

/*---------------------------------------------------------------------------*/

static void
doDrawTriangleEdge (g, visual, qe)

     graphType * g;
     visualType * visual;
     indexType qe;

{
  doDrawTriangle (g, visual, (*visual->org) (g, qe), (*visual->dst) (g, qe), (*visual->dst) (g, (*visual->onext) (g, qe)));
}

/*---------------------------------------------------------------------------*/

static void
drawTriangleColor (g, visual, qe, actcolor)

     graphType * g;
     visualType * visual;
     indexType qe;
     colorType actcolor;

{
  COLOR (actcolor);
  doDrawTriangleEdge (g, visual, qe);
}	

/*---------------------------------------------------------------------------*/

static void
drawTriangleShading (g, visual, qe)

     graphType * g;
     visualType * visual;
     indexType qe;

{
  indexType org, dst, dst2;
  float ftmp[3];

  org = (*visual->org) (g, qe);
  dst = (*visual->dst) (g, qe);
  dst2 = (*visual->dst) (g, (*visual->onext) (g, qe));

  crossProduct (g, visual, org, dst, dst2, ftmp);
  N3F (ftmp);
  doDrawTriangle (g, visual, org, dst, dst2);
}	

/*---------------------------------------------------------------------------*/

static void
doDrawEdge (g, visual, org, dst)

     graphType * g;
     visualType * visual;
     indexType org, dst;

{
  long tmp[3];

  BGNLINE (); {
    tmp[0] = SITEX (g, org);
    tmp[1] = SITEY (g, org);
    tmp[2] = (*visual->getThirdCoord) (g, org);
    V3I (tmp);
    tmp[0] = SITEX (g, dst);
    tmp[1] = SITEY (g, dst);
    tmp[2] = (*visual->getThirdCoord) (g, dst);
    V3I (tmp);
  } ENDLINE ();
}

/*---------------------------------------------------------------------------*/

static void
doDrawEdgeEdge (g, visual, qe)

     graphType * g;
     visualType * visual;
     indexType qe;

{
  doDrawEdge (g, visual, (*visual->org) (g, qe), (*visual->dst) (g, qe));
}

/*---------------------------------------------------------------------------*/

static void
drawEdgeColor (g, visual, qe, actcolor)

     graphType * g;
     visualType * visual;
     indexType qe;
     colorType actcolor;

{
  COLOR (actcolor);
  doDrawEdgeEdge (g, visual, qe);
}

/*---------------------------------------------------------------------------*/

void 
quality2dRedrawEdge (g, visual, theQe, cCode)
     
     graphType * g;
     visualType * visual;
     indexType theQe;
     int       cCode;

{
  float actRatio;
  colorType actColor;
  indexType qe;

  qe = theQe;
  
  if (! (*visual->isDeleted) (g, (*visual->qetoe) (qe))) {
    do {
      if ((*visual->dst) 
	  (g, (*visual->onext) (g, qe)) == 
	  (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) {
	if (cCode != NULL) {
	  actRatio = MAX ((*getColor) (g, visual, qe), 
			  (*getColor) 
			  (g, visual, (*visual->sym) 
			   ((*visual->onext) (g, qe))));
	  actRatio = MAX (actRatio, 
			  (*getColor) 
			  (g, visual, 
			   (*visual->oprev) (g, (*visual->sym) (qe))));
	  makeColor (actRatio, actColor);
	} else {
	  actColor [0] = 0; actColor [1] = 0; actColor [2] = 0;
	}
	
	FRONTBUFFER ((Boolean) 1);
	drawTriangleColor (g, visual, qe, actColor);
	FRONTBUFFER ((Boolean) 0);
      }
      qe = (*visual->sym) (qe);
    } while (qe != theQe);
  }
  
/*  FRONTBUFFER ((Boolean) 1);
  showQualityBar (g, visual);
  FRONTBUFFER ((Boolean) 0);*/
}

/*---------------------------------------------------------------------------*/

void
quality2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  indexType edge;
  indexType qe;
  long saveWindow;
  float actRatio;
  colorType actColor;
  
  saveWindow = WINGET ();
  WINSET (visual->window);
  
  ZBUFFER (TRUE);
  COLOR (visual->colorTable[0]);
  CLEAR ();
  
  for (edge = 0; edge < NE(g); edge++) {
    qe = (*visual->makeqe) (edge);
    if (! (*visual->isDeleted) (g, edge)) {
      do {
	actRatio = (*getColor) (g, visual, qe);
	if ((! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->onext) (g, qe)))) &&
	    (! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->oprev) (g, (*visual->sym) (qe))))) &&
            ((*visual->dst) (g, (*visual->onext) (g, qe)) == (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) &&
	    (actRatio >= (*getColor) (g, visual, (*visual->sym) ((*visual->onext) (g, qe)))) &&
	    (actRatio >= (*getColor) (g, visual, (*visual->oprev) (g, (*visual->sym) (qe))))) {
	
	  makeColor (actRatio, actColor);
	  drawTriangleColor (g, visual, qe, actColor);
	}
	qe = (*visual->sym) (qe);
      } while (qe != (*visual->makeqe) (edge));
    }
  }
  
  visual->drawVertices (g, visual);

/*  showQualityBar (g, visual);*/
  SWAPBUFFERS ();
  ZBUFFER (FALSE);
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void 
stdt2dRedrawEdge (g, visual, qe, cCode)
     
     graphType * g;
     visualType * visual;
     indexType qe;
     int       cCode;

{
  if (! (*visual->isDeleted) (g, (*visual->qetoe) (qe))) {
    FRONTBUFFER ((Boolean) 1);
    drawEdgeColor (g, visual, qe, visual->colorTable [cCode]);
    FRONTBUFFER ((Boolean) 0);
  }
}

/*---------------------------------------------------------------------------*/

void
hiddenLineA2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  indexType edge;
  indexType qe;
  long saveWindow;
  indexType org, dst, dst2;
  short rMask, gMask, bMask;

  saveWindow = WINGET ();
  WINSET (visual->window);

  ZBUFFER (TRUE);

  COLOR (visual->colorTable[0]);
  CLEAR ();
  GRGBMASK(&rMask, &gMask, &bMask);

  for (edge = 0; edge < NE(g); edge++) {
    if (! (*visual->isDeleted) (g, edge)) {
      qe = (*visual->makeqe) (edge);
      do {
	if ((! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->onext) (g, qe)))) &&
	    (! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->oprev) (g, (*visual->sym) (qe))))) &&
	    ((*visual->dst) (g, (*visual->onext) (g, qe)) == (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) &&
	    (qe < (*visual->sym) ((*visual->onext) (g, qe))) &&
	    (qe < (*visual->oprev) (g, (*visual->sym) (qe)))) {
	  ZWRITEMASK(0);
	  org = (*visual->org) (g, qe);
          dst = (*visual->dst) (g, qe);
          dst2 = (*visual->dst) (g, (*visual->onext) (g, qe));

	  drawTriangleColor (g, visual, qe, visual->colorTable[0]);

	  COLOR (visual->colorTable[1]);
	  doDrawEdge (g, visual, org, dst);
	  doDrawEdge (g, visual, org, dst2);
	  doDrawEdge (g, visual, dst, dst2);

	  ZWRITEMASK(0xFFFFFFFF);
	  RGBWRITEMASK(0,0,0);
	  doDrawTriangle (g, visual, org, dst, dst2);
	  RGBWRITEMASK(rMask, gMask, bMask);
	}
	qe = (*visual->sym) (qe);
      } while (qe != (*visual->makeqe) (edge));
    }
  }

  visual->drawVertices (g, visual);

  SWAPBUFFERS ();
  ZBUFFER (FALSE);
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void
hiddenLineB2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  indexType edge;
  indexType qe;
  long saveWindow;
  short rMask, gMask, bMask;
  Matrix old;

  saveWindow = WINGET ();
  WINSET (visual->window);

  ZBUFFER (TRUE);

  COLOR (visual->colorTable[0]);
  CLEAR ();
  GRGBMASK(&rMask, &gMask, &bMask);

  for (edge = 0; edge < NE(g); edge++) {
    if (! (*visual->isDeleted) (g, edge)) {
      qe = (*visual->makeqe) (edge);
      do {
	if ((! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->onext) (g, qe)))) &&
	    (! (*visual->isDeleted) (g, (*visual->qetoe) ((*visual->oprev) (g, (*visual->sym) (qe))))) &&
	    ((*visual->dst) (g, (*visual->onext) (g, qe)) == (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) &&
	    (qe < (*visual->sym) ((*visual->onext) (g, qe))) &&
	    (qe < (*visual->oprev) (g, (*visual->sym) (qe)))) {
	  drawTriangleColor (g, visual, qe, visual->colorTable[0]);
	}
	qe = (*visual->sym) (qe);
      } while (qe != (*visual->makeqe) (edge));
    }
  }

  ZWRITEMASK (0);
  PUSHMATRIX ();
  GETMATRIX (old);
  LOADMATRIX (ident);
  TRANSLATE (0, 0, 0.005 * MAX (visual->x1 - visual->x0,
				 visual->y1 - visual->y0));
  MULTMATRIX (old);

  COLOR (visual->colorTable [1]);
  for (edge = 0; edge < NE(g); edge++) {
    if (! (*visual->isDeleted) (g, edge)) {
      doDrawEdgeEdge (g, visual, (*visual->makeqe) (edge));
    }
  }

  POPMATRIX ();
  ZWRITEMASK (0xFFFFFFFF);

  visual->drawVertices (g, visual);

  SWAPBUFFERS ();
  ZBUFFER (FALSE);
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void
shading2dRedrawEdge (g, visual, theQe, cCode)

     graphType * g;
     visualType * visual;
     indexType theQe;
     int       cCode;

{
  indexType qe;
  colorType actColor;

  qe = theQe;

  if (! (*visual->isDeleted) (g, (*visual->qetoe) (qe))) {
    FRONTBUFFER ((Boolean) 1);
    if (cCode == NULL) {
      actColor [0] = 0; actColor [1] = 0; actColor [2] = 0;
      COLOR (actColor);
    }

    do {
      if (((*visual->dst) 
	  (g, (*visual->onext) (g, qe)) == 
	  (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) 
	  && ((*visual->dst) (g, (*visual->onext) (g, qe)) >= 0)) {
	if (cCode != NULL) 
	  drawTriangleShading (g, visual, qe);
	else 
	  doDrawTriangleEdge (g, visual, qe);
      }
      qe = (*visual->sym) (qe);
    } while (qe != theQe);
    
    COLOR (visual->colorTable [2]);

    FRONTBUFFER ((Boolean) 0);
  }
}

/*---------------------------------------------------------------------------*/

void
shading2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  indexType edge;
  indexType qe;
  long saveWindow;

  saveWindow = WINGET ();
  WINSET (visual->window);

  ZBUFFER (TRUE);

  COLOR (visual->colorTable[0]);
  CLEAR ();

  NMODE (NAUTO);

  for (edge = 0; edge < NE(g); edge++) {
    qe = (*visual->makeqe) (edge);
    if (! (*visual->isDeleted) (g, edge)) {
      do {
	if ((! (*visual->isDeleted) 
	     (g, (*visual->qetoe) ((*visual->onext) (g, qe)))) &&
	    (! (*visual->isDeleted) 
	     (g, (*visual->qetoe) ((*visual->oprev) 
				   (g, (*visual->sym) (qe))))) &&
	    ((*visual->dst) 
	     (g, (*visual->onext) (g, qe)) == 
	     (*visual->dst) (g,  (*visual->oprev) (g, (*visual->sym) (qe)))) &&
	    (qe < (*visual->sym) ((*visual->onext) (g, qe))) &&
	    (qe < (*visual->oprev) (g, (*visual->sym) (qe)))) {

	  drawTriangleShading (g, visual, qe);
/*	  drawEdgeColor (g, visual, qe, visual->colorTable [2]);*/
	}
	qe = (*visual->sym) (qe);
      } while (qe != (*visual->makeqe) (edge));
    }
  }
  
  visual->drawVertices (g, visual);

  SWAPBUFFERS ();
  ZBUFFER (FALSE);
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void
wireframe2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  indexType edge;
  long saveWindow;

  saveWindow = WINGET ();
  WINSET (visual->window);

  COLOR (visual->colorTable[0]);
  CLEAR ();

  COLOR (visual->colorTable [1]);
  for (edge = 0; edge < NE(g); edge++) {
    if (! (*visual->isDeleted) (g, edge)) {
      doDrawEdgeEdge (g, visual, (*visual->makeqe) (edge));
    }
  }

  visual->drawVertices (g, visual);

  SWAPBUFFERS ();
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/

void
vertices2dRedrawWindow (g, visual)

     graphType * g;
     visualType * visual;

{
  long saveWindow;

  saveWindow = WINGET ();
  WINSET (visual->window);

  COLOR (visual->colorTable[0]);
  CLEAR ();

  drawVerticesTetrahedra (g, visual);

  SWAPBUFFERS ();
  WINSET (saveWindow);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/







