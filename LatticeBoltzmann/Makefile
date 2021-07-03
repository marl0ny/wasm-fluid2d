SHELL = /bin/bash

COMPILE = /usr/bin/clang++
FLAGS = -O0 -Wall -g -std=c++11
# COMPILE = /usr/bin/g++ 
# FLAGS = -Og -Wall -g

INCLUDE = -I${PWD}/gl_wrappers2d
LIBS = -lm -lGL -lGLEW -lglfw

TARGETDIR = .

TARGET = ${TARGETDIR}/fluid2d
SOURCES = fluid2d.cpp ./gl_wrappers2d/gl_wrappers.cpp
OBJECTS = fluid2d.o gl_wrappers.o

TARGET2 = ${TARGETDIR}/fluid.js
MERGE = main.cpp
SCRIPT = merge.py

all: ${TARGET} # ${TARGET2}

${TARGET}: ${OBJECTS}
	${COMPILE} ${FLAGS} -o $@ $^ ${LIBS}

${OBJECTS}: ${SOURCES}
	${COMPILE} ${FLAGS} -c $^ ${INCLUDE}

${TARGET2}: ${SCRIPT} ${SOURCES}
	python3 ${SCRIPT}
	emcc -o $@ ${MERGE} -std=c++11 -O3 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1
	rm -f ${MERGE}

clean:
	rm -f *.o *~ *.wasm *.mem *.js main.cpp ${TARGET}

