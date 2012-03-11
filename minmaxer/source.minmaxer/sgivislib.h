#include <gl.h>
/*---------------------------------------------------------------------------*/

extern long openWindow ();
extern long top2dWindow ();
extern long corner2dWindow ();
extern long centered2dWindow ();
extern long general2dWindow ();

#define MINSIZE(a, b) minsize((long) a, (long) b)
#define MAXSIZE(a, b) maxsize((long) a, (long) b)
#define NMODE(a) nmode((long) a)
#define LMDEF(a, b, c, d) lmdef((short) a, (short) b, (short) c, d)
#define LMBIND(a, b) lmbind((short) a, (short) b)
#define DEPTHCUE(a) depthcue((short) a)
#define PERSPECTIVE(a, b, c, d) perspective((Angle) a, (float) b,\
                                            (Coord) c, (Coord) d)
#define GRGBMASK(a, b, c) gRGBmask(a, b, c)
#define RGBWRITEMASK(a, b, c) RGBwritemask((short) a, (short) b, (short) c)
#define ZWRITEMASK(a) zwritemask((unsigned long) a)
#define LOADMATRIX(a) loadmatrix(a)
#define GETMATRIX(a) getmatrix(a)
#define MULTMATRIX(a) multmatrix(a)
#define PUSHMATRIX() pushmatrix()
#define POPMATRIX() popmatrix()
#define ZFUNCTION(a) zfunction ((long) a)
#define LOOKAT(a, b, c, d, e, f, g) lookat((Coord)a,(Coord)b,(Coord)c,\
                                           (Coord)d,(Coord)e,(Coord)f,\
					   (Angle)g)
#define ROTATE(a, b) rotate((Angle) a, (char) b)
#define TRANSLATE(a, b, c) translate((Coord) a, (Coord) b, (Coord) c)
#define MMODE(a) mmode(a)
#define RGBMODE() RGBmode()
#define BACKBUFFER(a) backbuffer(a)
#define CLEAR() do {clear(); zclear();} while (0)
#define COLOR(b) c3f(b)
#define COLORF(b) c3f(b)
#define DOUBLEBUFFER() doublebuffer()
#define FOREGROUND() foreground()
#define FRONTBUFFER(a) frontbuffer(a)
#define GCONFIG() gconfig()
#define ORTHO2(a, b, c, d) ortho2((Coord) a, (Coord) b, (Coord) c, (Coord) d)
#define ORTHO(a, b, c, d, e, f) ortho((Coord) a, (Coord) b, (Coord) c,\
                                      (Coord) d, (Coord) e, (Coord) f)
#define QENTER(a, b) qenter((Device) a, (short) b)
#define WINGET() winget()
#define GETBUTTON(a) getbutton((Device) a)
#define WINSET(a) winset(a)
#define SWAPBUFFERS() swapbuffers()
#define RECTFI(a, b, c, d)  rectfi(a, b, c, d)
#define GETORIGIN(a, b) getorigin((long *) a, (long *) b)
#define GETSIZE(a, b) getsize((long *) a, (long *) b)
#define CMOV2I(a, b) cmov2i(a, b)
#define CHARSTR(a) charstr(a)
#define GETCPOS(a, b) getcpos(a, b)
#define QDEVICE(a) fl_qdevice(a)
#define TIE(a, b, c) tie(a, b, c)
#define QRESET() qreset()
#define GETVALUATOR(a) getvaluator(a)
#define QREAD(a) fl_qread((short *) a)
#define PREFPOSITION(a, b, c, d) prefposition(a, b, c, d)
#define WINPOSITION(a, b, c, d) winposition(a, b, c, d)
#define WINOPEN(a) winopen(a)
#define WINTITLE(a) wintitle(a)
#define SHADEMODEL(a) shademodel(a)
#define BGNLINE bgnline
#define ENDLINE endline
#define BGNTMESH bgntmesh
#define ENDTMESH endtmesh
#define BGNPOLYGON bgnpolygon
#define ENDPOLYGON endpolygon
#define V2S(a) v2s(a)
#define V2I(a) v2i(a)
#define V3S(a) v3s(a)
#define V3I(a) v3i(a)
#define N3F(a) n3f(a)
#define C3F(a) c3f(a)
#define ZBUFFER(a) zbuffer((Boolean) a)
#define MOVE2I(a, b) move2i(a, b)
#define DRAW2I(a, b) draw2i(a, b)
