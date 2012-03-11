# 1 "theMakefile"
SOURCE = ../source.triangulation



 











MACHINE_TYPE = sun

RMEXECUTABLE = rm minmaxer;



 

# 35 "theMakefile"


INCLUDE = -I$(SOURCE)
DEBUG = -O
CFLAGS = 
LIB = -lm
LIBDIR = 



CC = cc $(CFLAGS) $(DEBUG)  $(INCLUDE) -o $@

.SUFFIXES: .$(OBJECT) $(SUFFIXES)
.c.$(OBJECT): 
	$(CC)

 




VISUALOBJ = sunvisual.o 


TRIANGULATIONMINIMUMOBJ = $(SOURCE)/novisual.o $(SOURCE)/queue.o $(SOURCE)/heap.o $(SOURCE)/stack.o $(SOURCE)/timer.o $(SOURCE)/bitvector.o $(SOURCE)/file_io.o $(SOURCE)/quicksort.o $(SOURCE)/longmath.o $(SOURCE)/graph.o $(SOURCE)/quadedge.o $(SOURCE)/triangulation.o

ALLTRIOBJ = $(SOURCE)/sos.o $(SOURCE)/planesweep.o $(SOURCE)/flips.o $(SOURCE)/delaunay.o $(SOURCE)/persistent.quadedge.o $(SOURCE)/hdag.o $(SOURCE)/regular.o $(SOURCE)/edgeinsert.o $(SOURCE)/heuristic.angle.o $(SOURCE)/angle.o $(SOURCE)/heuristic.slope.o $(SOURCE)/slope.o $(SOURCE)/heuristic.height.o $(SOURCE)/height.o

MINMAXEROBJ = $(TRIANGULATIONMINIMUMOBJ) $(ALLTRIOBJ) support.o minmaxer.o

minmaxer: $(MINMAXEROBJ) Makefile
	cc  $(DEBUG) -o minmaxer $(LIBDIR) $(MINMAXEROBJ) $(LIB)

 
 
 


