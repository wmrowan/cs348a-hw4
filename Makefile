tour: tour.cpp
	g++ tour.cpp -o tour

tags:
	ctags -R .

clean:
	rm -f tour tags
