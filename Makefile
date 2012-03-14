LIB = -lglut -lGL -lGLU -lfltk_gl -lfltk
CPPOPTS = -g
CC = g++ $< $(CPPOPTS) $(LIB) -o $@ 

tour: tour.cpp
	$(CC)

tags:
	ctags -R .

clean:
	rm -f tour tags regular.tri
