INSTALL_PATH?=/usr/local
CC=gcc
CXX=g++

all: objects
	${CXX} --std=c++11 *.cpp objects/*.o -o bake

objects:
	mkdir -p objects/
	${CC} sundown/*.c -c
	mv *.o objects/

install:
	cp bake ${INSTALL_PATH}/bin

clean:
	rm -r objects/
	rm bake
