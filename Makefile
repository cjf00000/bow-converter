all: converter
CPPFLAGS=-fopenmp -O3 -std=c++11

converter: corpus.o dataset.o main.o readbuf.o vocabulary.o
	g++ $(CPPFLAGS) -g -o $@ $^

%.o: %.cpp
	g++ $(CPPFLAGS) -g -c -o $@ $^

clean:
	-rm *.o converter

.PHONY: clean
