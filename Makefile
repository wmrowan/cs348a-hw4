LIB = -lGL -lGLU -lglut
CPPOPTS = -g
CC = g++ $(CPPOPTS) $(LIB) -o $@ $<

tour: tour.cpp
	$(CC)

run: tour
	./tour test.tri

tags:
	ctags -R .

clean:
	rm -f tour tags
