all: Bralab

Bralab: Bralab.o
	g++ Bralab.o -o Bralab -std=c++0x `pkg-config gtkmm-3.0 --cflags --libs` 

Bralab.o: Bralab.cpp
	g++ -c Bralab.cpp -std=c++0x `pkg-config gtkmm-3.0 --cflags --libs`

clean:
	rm -f *.o
