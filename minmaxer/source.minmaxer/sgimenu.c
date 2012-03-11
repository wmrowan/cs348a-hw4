#include <gl.h>
#include <device.h>
#include "road.h"
#include "graph.h"
#include "sgivislib.h"
#include "menu.h"
#include "visual.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

makeInitialMenuLook (visual)

     visualType * visual;

{
  fl_freeze_form (visual->menu);
  fl_hide_object (minmaxerInputGroup);
  fl_hide_object (minmaxerComputationGroup);
  fl_show_object (minmaxerInfoGroup);
  fl_show_object (minmaxerAlgorithmsGroup);
  fl_show_object (minmaxerPropertiesGroup);
  fl_show_object (minmaxerRenderGroup);
  fl_set_button (b201, 1);
  fl_set_button (b311, 1);
  fl_set_button (b209, 1);
  fl_set_button (b308, 1);
  fl_set_slider_bounds (b302, (float) 0.0, (float) 3.0);
  fl_set_slider_value (b302, (float) 1.0);
  fl_unfreeze_form (visual->menu);
}

/*---------------------------------------------------------------------------*/

makeAlgorithmsMenuLook (visual)

     visualType * visual;

{
  fl_freeze_form (visual->menu);
  fl_hide_object (minmaxerInputGroup);
  fl_hide_object (minmaxerComputationGroup);
  fl_show_object (minmaxerAlgorithmsGroup);
  fl_show_object (minmaxerPropertiesGroup);
  fl_unfreeze_form (visual->menu);
}

/*---------------------------------------------------------------------------*/

makeComputationMenuLook (visual)

     visualType * visual;

{
  fl_freeze_form (visual->menu);
  fl_hide_object (minmaxerInputGroup);
  fl_hide_object (minmaxerAlgorithmsGroup);
  fl_show_object (minmaxerComputationGroup);
  fl_show_object (minmaxerPropertiesGroup);
  fl_unfreeze_form (visual->menu);
}

/*---------------------------------------------------------------------------*/

makeSaveFileMenuLook (visual)

     visualType * visual;

{
  fl_freeze_form (visual->menu);
  fl_hide_object (minmaxerPropertiesGroup);
  fl_hide_object (minmaxerComputationGroup);
  fl_show_object (minmaxerAlgorithmsGroup);
  fl_show_object (minmaxerInputGroup);
  fl_unfreeze_form (visual->menu);
}

/*---------------------------------------------------------------------------*/

static int
setRenderFunction (g, visual, button)

     graphType *g;
     visualType *visual;
     FL_OBJECT *button;

{
  int isRenderButton;

  isRenderButton = 1;

  if (button == b200) { /* Draw Vertices Only */
    visual->redrawWindow = vertices2dRedrawWindow;
    visual->redrawEdge = dummyFun2;
  } else if (button == b201) { /* Draw Wireframe */
    visual->redrawWindow = wireframe2dRedrawWindow;
    visual->redrawEdge = stdt2dRedrawEdge;
  } else if (button == b202) { /* Draw Hidden Line 2 */
    visual->redrawWindow = hiddenLineA2dRedrawWindow;
    visual->redrawEdge = stdt2dRedrawEdge;
  } else if (button == b203) { /* Draw Hidden Line 1 */
    visual->redrawWindow = hiddenLineB2dRedrawWindow;
    visual->redrawEdge = stdt2dRedrawEdge;
  } else if (button == b204) { /* Draw Surface Shading */
    visual->redrawWindow = shading2dRedrawWindow;
    visual->redrawEdge = shading2dRedrawEdge;
  } else if (button == b205) { /* Draw Minimum Angle */
    quality2dSetFunctions (g, visual, getColorMinAngle);
    visual->redrawWindow = quality2dRedrawWindow;
    visual->redrawEdge = quality2dRedrawEdge;
  } else if (button == b206) { /* Draw Maximum Angle */
    quality2dSetFunctions (g, visual, getColorMaxAngle);
    visual->redrawWindow = quality2dRedrawWindow;
    visual->redrawEdge = quality2dRedrawEdge;
  } else if (button == b207) { /* Draw Minimum Height */
    quality2dSetFunctions (g, visual, getColorMinHeight);
    visual->redrawWindow = quality2dRedrawWindow;
    visual->redrawEdge = quality2dRedrawEdge;
  } else if (button == b208) { /* Draw Maximum Slope */
    quality2dSetFunctions (g, visual, getColorMaxSlope);
    visual->redrawWindow = quality2dRedrawWindow;
    visual->redrawEdge = quality2dRedrawEdge;
  } else if (button == b209) { /* Don't draw anything */
    if (fl_get_button (b209)) {
      visual->eraseEdge = visual->memorizeEraseEdge;
      visual->drawEdge = visual->memorizeDrawEdge;
    } else {
      visual->memorizeEraseEdge = visual->eraseEdge;
      visual->memorizeDrawEdge = visual->drawEdge;
      visual->eraseEdge = stdt2dNoDraw;
      visual->drawEdge = stdt2dNoDraw;
    }
  } else
    isRenderButton = 0;
  
  if (isRenderButton == 1) {
    if (! fl_get_button (b308))
      visual->rotateRedraw = visual->redrawWindow;
  }
  (*visual->redrawWindow) (g, visual);

  return isRenderButton;
}

/*---------------------------------------------------------------------------*/

static int
setPropertyFunction (g, visual, button)

     graphType *g;
     visualType *visual;
     FL_OBJECT *button;

{
  int isPropertyButton;

  isPropertyButton = 1;

  if (button == b307) {
    if (fl_get_button (b307))
      visual->autoRotateView = autoRotateView;
    else
      visual->autoRotateView = dummyFun2;
  } else if (button == b304) { /* Draw Sloppily <-> Draw Nicely */
    (*visual->redrawWindow) (g, visual);
    if (fl_get_button (b304)) 
      if (fl_get_button (b209)) {
	visual->eraseEdge = stdt2dEraseEdge;
	visual->drawEdge = stdt2dDrawEdge;
      } else {
	visual->memorizeEraseEdge = stdt2dEraseEdge;
	visual->memorizeDrawEdge = stdt2dDrawEdge;
      }
    else 
      if (fl_get_button (b209)) {
	visual->eraseEdge = stdt2dEraseNiceEdge;
	visual->drawEdge = stdt2dDrawNiceEdge;
      } else {
	visual->memorizeEraseEdge = stdt2dEraseNiceEdge; 
        visual->memorizeDrawEdge = stdt2dDrawNiceEdge;
      }
  } else if (button == b308) { /* Auto Wireframe Off <-> Auto Wireframe On */
    if (fl_get_button (b308))
      visual->rotateRedraw = wireframe2dRedrawWindow;
    else
      visual->rotateRedraw = visual->redrawWindow;
  } else if (button == b311) { /* Show Vertices <-> Don't Show Vertices */
    if (! fl_get_button (b311))
      visual->drawVertices = dummyFun2;
    else
      visual->drawVertices = drawVerticesTetrahedra;
    (*visual->redrawWindow) (g, visual);
  } else if (button == b310) { /*Weights(Z-coord) Off <-> Weights(Z-coord) On */
    if (! (fl_get_button (b310)))
      if ((visual->getThirdCoord == getX2Y2minusZ) ||
          (visual->getThirdCoord == getX2Y2)) {
        visual->getThirdCoord = getX2Y2minusZ;
        initX2Y2Zcoord (g, visual);
      } else {
        visual->getThirdCoord = getZ;
        initZcoord (g, visual);
      }
    else
      if ((visual->getThirdCoord == getX2Y2minusZ) ||
          (visual->getThirdCoord == getX2Y2)) {
        visual->getThirdCoord = getX2Y2;
        initX2Y2coord (g, visual);
      } else {
        visual->getThirdCoord = get0;
        initZcoord (g, visual);
      }
    (*visual->redrawWindow) (g, visual);
  } else if (button == b309) { /* Lifting Map Off <-> Lifting Map On */
    if (fl_get_button (b309)) {
      if ((visual->getThirdCoord == getZ) ||
          (visual->getThirdCoord == getX2Y2minusZ)) {
        visual->getThirdCoord = getX2Y2minusZ;
        initX2Y2Zcoord (g, visual);
      } else {
        visual->getThirdCoord = getX2Y2;
        initX2Y2coord (g, visual);
      }
    } else {
      initZcoord (g, visual);
      if ((visual->getThirdCoord == getZ) ||
          (visual->getThirdCoord == getX2Y2minusZ))
        visual->getThirdCoord = getZ;
      else
        visual->getThirdCoord = get0;
    }
    (*visual->redrawWindow) (g, visual);
  } else
    isPropertyButton = 0;

  return isPropertyButton;
}

/*---------------------------------------------------------------------------*/

static int
setAlgorithmFunction (g, visual, button)

     graphType *g;
     visualType *visual;
     FL_OBJECT *button;

{
  if (button == b100) { /* Quit */
    return -1;
  } else if (button == b101) { /* Beneath Beyond */
    fl_set_object_label (bAlgorithm, "Initial (Plane Sweep)");
    return 1;
  } else if (button == b102) { /* Delaunay */
    fl_set_object_label (bAlgorithm, "Delaunay (Lawson Flip)");
    return 2;
  } else if (button == b103) { /* MinMax Angle (n^2 log n) */
    fl_set_object_label (bAlgorithm, "MinMax Angle (n^2 log n)");
    return 3;
  } else if (button == b104) { /* MinMax Angle (n^3) */
    fl_set_object_label (bAlgorithm, "MinMax Angle (n^3)");
    return 4;
  } else if (button == b105) { /* MaxMin Height (n^2 log n) */
    fl_set_object_label (bAlgorithm, "MaxMin Height (n^2 log n)");
    return 5;
  } else if (button == b106) { /* MaxMin Height (n^3) */
    fl_set_object_label (bAlgorithm, "MaxMin Height (n^3)");
    return 6;
  } else if (button == b107) { /* MinMax Slope (n^3) */
    fl_set_object_label (bAlgorithm, "MinMax Slope (n^3)");
    return 7;
  } else if (button == b108) { /* Delaunay (Incremental) */
    fl_set_object_label (bAlgorithm, "Delaunay (Incremental Flip)");
    return 8;
  } else if (button == b109) { /* Regular (Incremental) */
    fl_set_object_label (bAlgorithm, "Regular (Incremental Flip)");
    return 9;
  } else if (button == b110) { /* Remove Duplicate Points */
    fl_set_object_label (bAlgorithm, "Remove Duplicate Vertices");
    return 12;
  } else if (button == b111) { /* Save Triangulation */
    fl_set_object_label (bAlgorithm, "Save Triangulation");
    return 13;
  } else
    return 0;
}

/*---------------------------------------------------------------------------*/

static int
setComputationFunction (g, visual, button)

     graphType *g;
     visualType *visual;
     FL_OBJECT *button;

{
  if (button == b303) { /* Pause <-> Continue */
    if (fl_get_button (b303))
        visual->timeSup = returnFalse;
    else 
      visual->timeSup = outOfTime;
    return -1;
  } else if (button == b306) { /* Step */
    return 1;
  } else if (button == b400) { /* Interrrupt */
    DO_INTERRUPT_ALGORITHM;
    visual->timeSup = outOfTime;
    fl_set_button (b303, 0);
    return 1;
  } else if (button == b302) { /* Change in Slider Position */
    visual->delayTime = 3.0 - fl_get_slider_value (b302);
    return -1;
  } else
    return 0;
}

/*---------------------------------------------------------------------------*/

menuType *
makeMainMenu (visual, g)

     visualType * visual;
     graphType * g;

{
  menuType * tmp;
  static char s[12];

  fl_init ();
  visual->menu = tmp = create_form_menuForm ();
  makeInitialMenuLook (visual);
  sprintf (s, "%d", NS(g));
  fl_set_object_label (bNofVertices, s);
  fl_set_object_label (bFname, g->fileName);
  fl_set_object_label (bNofEdges, "");
  fl_set_object_label (bNofCHedges, "");
  fl_set_form_position (visual->menu, 1003, 55);
  fl_show_form (visual->menu, FL_PLACE_POSITION,TRUE," ");

  QDEVICE (LEFTMOUSE);
  TIE (LEFTMOUSE, MOUSEX, MOUSEY);
  QDEVICE (RIGHTMOUSE);
  TIE (RIGHTMOUSE, MOUSEX, MOUSEY);
  QDEVICE (MIDDLEMOUSE);
  TIE (MIDDLEMOUSE, MOUSEX, MOUSEY);

  return tmp;
}

/*---------------------------------------------------------------------------*/

void
resetChoice (m, visual, g)

     menuType * m;
     visualType * visual;
     graphType * g;

{
  static char s[20], t[20];

  sprintf (s, "%d", NE(g));
  fl_set_object_label (bNofEdges, s);
  sprintf (t, "%d", (g->nChEdges < 0 ? 0 : g->nChEdges));
  fl_set_object_label (bNofCHedges, t);

  makeAlgorithmsMenuLook (visual);
  visual->getChoiceButton = noAlgorithmLoop;

  assignQEfunctions (visual);

  (*visual->redrawWindow) (g, visual);
}

/*---------------------------------------------------------------------------*/

int
algorithmLoop (m, visual, g)

     menuType * m;
     visualType * visual;
     graphType * g;

{
  FL_OBJECT *button;
  int val;

  do {
    button = fl_check_forms ();

    if (button != NULL)
      if (button == FL_EVENT) /* big window needs attention */
	doBigWindowAction (g, visual);
      else
	if (val = setComputationFunction (g, visual, button)) {
	  if (val > 0)
	    return val;
	} else
	  if (! (val = setRenderFunction (g, visual, button)))
	    val = setPropertyFunction (g, visual, button);
    (*visual->autoRotateView) (visual, g);
  } while (! ((*visual->timeSup) (visual->delayTime)));
  return 1;
}

/*---------------------------------------------------------------------------*/

int
noAlgorithmLoop (m, visual, g)

     menuType * m;
     visualType * visual;
     graphType * g;

{
  FL_OBJECT *button;
  int val;

  do {
    button = fl_check_forms ();

    if (button != NULL)
      if (button == FL_EVENT) /* big window needs attention */
	doBigWindowAction (g, visual);
      else
	if (val = setAlgorithmFunction (g, visual, button)) {
	  if ((val != 13) && (val != -1)) {
	    makeComputationMenuLook (visual);
	    visual->getChoiceButton = algorithmLoop;
	  } else
	    if (val != -1)
	      makeSaveFileMenuLook (visual);
	  return (val != -1 ? val : 0);
	} else
	  if (! (val = setRenderFunction (g, visual, button)))
	    val = setPropertyFunction (g, visual, button);
    (*visual->autoRotateView) (visual, g);
  } while (1);
}

/*---------------------------------------------------------------------------*/

char *
getFileNameLoop (visual, g)

     visualType * visual;
     graphType * g;

{
  FL_OBJECT *button;
  int val;

  do {
    button = fl_check_forms ();

    if (button != NULL)
      if (button == FL_EVENT) /* big window needs attention */
	doBigWindowAction (g, visual);
      else
	if (! (val = setRenderFunction (g, visual, button))) 
	  if (button == b112) {
	    makeAlgorithmsMenuLook (visual);
	    return fl_get_input (b112);
	  } else 
	    if (button == b401) {
	      makeAlgorithmsMenuLook (visual);
	      return "";
	    }
    (*visual->autoRotateView) (visual, g);
  } while (1);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
