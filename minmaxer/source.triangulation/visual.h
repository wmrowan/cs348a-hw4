/*
 * NAME: visual.h
 *
*/

/*---------------------------------------------------------------------------*/

typedef float colorType [3];

#define COLORCOPY(c1, c2) do {(c2) [0] = (c1) [0];\
			      (c2) [1] = (c1) [1];\
			      (c2) [2] = (c1) [2];} while (0)

/*---------------------------------------------------------------------------*/

typedef struct {

  void (*initialize) ();
  void (*drawEdge) ();
  void (*redrawEdge) ();
  void (*eraseEdge) ();
  void (*memorizeEraseEdge) ();
  void (*memorizeDrawEdge) ();
  void (*memorizeRedrawWindow) ();
  void (*memorizeRedrawEdge) ();
  void (*cleanUp) ();
  void (*redrawWindow) ();
  menuType * (*makeMainMenu) ();
  menuType * (*makeChoiceMenu) ();
  void (*resetChoice) ();
  void (*pause) ();
  int (*getChoiceButton) ();
  void (*quality2dSetFunctions) ();
  void (*rotateRedraw) ();
  void (*autoRotateView) ();
  void (*drawVertices) ();
  long (*getThirdCoord) ();
  indexType (*dst) ();
  indexType (*org) ();
  indexType (*onext) ();
  indexType (*oprev) ();
  indexType (*sym) ();
  indexType (*qetoe) ();
  indexType (*makeqe) ();
  int (*isDeleted) ();
  void (*setDeleted) ();
  void (*unsetDeleted) ();
  void (*switchToPQE) ();
  int (*timeSup) ();
  char * (*getFileName) ();
  char * winTitle;
  int xLen, yLen;
  int x0, y0, x1, y1;
  int zMean;
  float scale;
  float trans[3], rvec[4];
  double COPx, COPy, COPz;
  float DeltaWOld, DeltaHOld, DeltaW, DeltaH;
  double maxDiff, ang;
  colorType colorTable[4];
  double delayTime;
  long window;
  menuType * menu;
  menuType * menu2;
  
} visualType;

/*---------------------------------------------------------------------------*/

extern visualType * dummyVisual ();
extern visualType * stdt2dVisual ();

#ifdef sgi
extern menuType * makeMainMenu ();
extern int outOfTime ();
extern int returnFalse ();
extern void stdt2dEraseEdge ();
extern void stdt2dDrawEdge ();
extern void stdt2dNoDraw ();
extern void stdt2dEraseNiceEdge ();
extern void stdt2dDrawNiceEdge ();
extern void autoRotateView ();
extern int dummyFun ();
extern void dummyFun2 ();
extern void wireframe2dRedrawWindow ();
extern void vertices2dRedrawWindow ();
extern void stdt2dRedrawEdge ();
extern void hiddenLineA2dRedrawWindow ();
extern void hiddenLineB2dRedrawWindow ();
extern void shading2dRedrawWindow ();
extern void shading2dRedrawEdge ();
extern void quality2dRedrawWindow ();
extern void quality2dRedrawEdge ();
extern void wirevertex2dRedrawWindow ();
extern void dummyFun2 ();
extern void drawVerticesTetrahedra ();
extern void quality2dSetFunctions ();
extern float getColorMinHeight ();
extern float getColorMaxSlope ();
extern float getColorMaxAngle ();
extern float getColorMinAngle ();
extern void resetChoice ();
extern long getX2Y2 ();
extern long getX2Y2minusZ ();
extern long getZ ();
extern long get0 ();
extern Matrix ident;
extern char * getFileNameLoop ();
#endif

/*---------------------------------------------------------------------------*/
#ifdef sgi
float (*getColor) ();
#endif

/*---------------------------------------------------------------------------*/

#define INTERRUPT_ALGORITHM do {if (GLOBAL_INTERRUPT_ALGORITHM == 1) goto INTERRUPT_LABEL;} while (0)
#define SHOULD_INTERRUPT_ALGORITHM (GLOBAL_INTERRUPT_ALGORITHM == 1)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
