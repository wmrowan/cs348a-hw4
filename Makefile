LIB = -lglut -lGL -lGLU -lsfml-graphics
CPPOPTS = -g
CC = g++ $< $(CPPOPTS) $(LIB) -o $@ 

tour: tour.cpp
	$(CC)

tags:
	ctags -R .

clean:
	rm -f tour tags regular.tri
