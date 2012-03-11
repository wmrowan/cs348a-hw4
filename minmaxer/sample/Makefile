
SOURCE = ../source.triangulation

#for sgi use
#CFLAGS = -cckr -signed -O
#INCLUDE = -I$(SOURCE) -I/usr/people/comba/xforms/xforms0.81/FORMS

#for non-sgi use
CFLAGS = -O
INCLUDE = -I$(SOURCE)

DEBUG =
LIB = -lm
LIBDIR =

CC = cc $(CFLAGS) $(DEBUG)  $(INCLUDE) -o $@

TRIANGULATIONMINIMUMOBJ = $(SOURCE)/novisual.o $(SOURCE)/queue.o $(SOURCE)/heap.o $(SOURCE)/stack.o $(SOURCE)/timer.o $(SOURCE)/bitvector.o $(SOURCE)/file_io.o $(SOURCE)/quicksort.o $(SOURCE)/longmath.o $(SOURCE)/graph.o $(SOURCE)/quadedge.o $(SOURCE)/triangulation.o

PLANESWEEPOBJ = $(SOURCE)/sos.o $(SOURCE)/planesweep.o

DELAUNAY1OBJ = $(SOURCE)/sos.o $(SOURCE)/flips.o $(SOURCE)/delaunay.o

REGULAROBJ = $(SOURCE)/sos.o $(SOURCE)/persistent.quadedge.o $(SOURCE)/hdag.o $(SOURCE)/regular.o
DELAUNAY2OBJ = $(REGULAROBJ)

MINMAXANGLEOBJ = $(SOURCE)/edgeinsert.o $(SOURCE)/heuristic.angle.o $(SOURCE)/angle.o
MINMAXSLOPEOBJ = $(SOURCE)/edgeinsert.o $(SOURCE)/heuristic.angle.o $(SOURCE)/heuristic.slope.o $(SOURCE)/slope.o
MAXMINHEIGHTOBJ = $(SOURCE)/edgeinsert.o $(SOURCE)/heuristic.angle.o $(SOURCE)/heuristic.height.o $(SOURCE)/height.o
ALLTRIOBJ = $(SOURCE)/sos.o $(SOURCE)/planesweep.o $(SOURCE)/flips.o $(SOURCE)/delaunay.o $(SOURCE)/persistent.quadedge.o $(SOURCE)/hdag.o $(SOURCE)/regular.o $(SOURCE)/edgeinsert.o $(SOURCE)/heuristic.angle.o $(SOURCE)/angle.o $(SOURCE)/heuristic.slope.o $(SOURCE)/slope.o $(SOURCE)/heuristic.height.o $(SOURCE)/height.o

TESTdelaunay1OBJ = $(TRIANGULATIONMINIMUMOBJ) $(DELAUNAY1OBJ) $(SOURCE)/planesweep.o testdelaunay1.o
TESTregularOBJ = $(TRIANGULATIONMINIMUMOBJ) $(DELAUNAY2OBJ) testregular.o
TESTangleOBJ = $(TRIANGULATIONMINIMUMOBJ) $(MINMAXANGLEOBJ) $(PLANESWEEPOBJ) testangle.o
TESTheightOBJ = $(TRIANGULATIONMINIMUMOBJ) $(MAXMINHEIGHTOBJ) $(PLANESWEEPOBJ) testheight.o
TESTslopeOBJ = $(TRIANGULATIONMINIMUMOBJ) $(MINMAXSLOPEOBJ) $(PLANESWEEPOBJ) testslope.o
TESTallOBJ = $(TRIANGULATIONMINIMUMOBJ) $(ALLTRIOBJ) testall.o

all: testdelaunay1 testregular testangle testheight testslope testall

testdelaunay1: $(TESTdelaunay1OBJ) Makefile
	cc  $(DEBUG) -o testdelaunay1 $(LIBDIR) $(TESTdelaunay1OBJ) $(LIB)
testregular: $(TESTregularOBJ) Makefile
	cc  $(DEBUG) -o testregular $(LIBDIR) $(TESTregularOBJ) $(LIB)
testangle: $(TESTangleOBJ) Makefile
	cc  $(DEBUG) -o testangle $(LIBDIR) $(TESTangleOBJ) $(LIB)
testheight: $(TESTheightOBJ) Makefile
	cc  $(DEBUG) -o testheight $(LIBDIR) $(TESTheightOBJ) $(LIB)
testslope: $(TESTslopeOBJ) Makefile
	cc  $(DEBUG) -o testslope $(LIBDIR) $(TESTslopeOBJ) $(LIB)
testall: $(TESTallOBJ) Makefile
	cc  $(DEBUG) -o testall $(LIBDIR) $(TESTallOBJ) $(LIB)

