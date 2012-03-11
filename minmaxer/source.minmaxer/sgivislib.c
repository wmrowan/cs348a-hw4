#include <gl.h>
#include <device.h>
#include "road.h"
#include "sgivislib.h"
#include "menu.h"
#include "visual.h"

/*---------------------------------------------------------------------------*/

long
openWindow (winTitle, x0, y0, deltaX, deltaY, winColor, shadeModel)

     char * winTitle;
     int x0, y0, deltaX, deltaY;
     colorType winColor;
     int shadeModel;

{
  long ok;

  FOREGROUND ();
  PREFPOSITION (x0, x0 + deltaX, y0, y0 + deltaY);
  ok = WINOPEN ("");
  WINTITLE (winTitle);
  COLOR (winColor);
  CLEAR ();
/*  SHADEMODEL(shadeModel);*/

  RGBMODE ();

  return ok;
}

/*---------------------------------------------------------------------------*/

long
general2dWindow (winTitle, deltaX, deltaY, x0, y0, winColor)

     char * winTitle;
     int deltaX, deltaY, x0, y0;
     colorType winColor;

{
  return openWindow (winTitle, x0, y0, deltaX, deltaY, winColor, FLAT);
}

/*---------------------------------------------------------------------------*/

long
top2dWindow (winTitle, deltaX, deltaY, x0, winColor)

     char * winTitle;
     int deltaX, deltaY, x0;
     colorType winColor;

{
  int bar = 31;
  return openWindow (winTitle, x0,
		     (YMAXSCREEN - deltaY - bar),
		     deltaX, deltaY, winColor, FLAT);
}

/*---------------------------------------------------------------------------*/

long
corner2dWindow (winTitle, deltaX, deltaY, winColor)

     char * winTitle;
     int deltaX, deltaY;
     colorType winColor;

{
  int bar = 31;
  return openWindow (winTitle, 8,
		     (YMAXSCREEN - deltaY - bar),
		     deltaX, deltaY, winColor, FLAT);
}

/*---------------------------------------------------------------------------*/

long
centered2dWindow (winTitle, deltaX, deltaY, winColor)

     char * winTitle;
     int deltaX, deltaY;
     colorType winColor;

{
  int bar = 31;
  return openWindow (winTitle, (XMAXSCREEN - deltaX) / 2, 
		     (YMAXSCREEN - deltaY - bar) / 2,
		     deltaX, deltaY, winColor, FLAT);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

    
