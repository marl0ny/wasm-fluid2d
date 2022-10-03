MACOSX_HOMEBREW = yes

SHELL = /bin/bash
FLAGS = -O0 -Wall -g
# FLAGS = -O2 -Wall
COMPILE = /usr/bin/clang


ifdef MACOSX_HOMEBREW
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers -I/opt/homebrew/include
LIBS = -ldl -L/opt/homebrew/lib -lglfw\
       -framework CoreVideo -framework OpenGL -framework IOKit\
       -framework Cocoa -framework Carbon
else
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers
LIBS = -lm -lGL -lGLEW -lglfw
endif

TARGET = ${PWD}/program
WASM_TARGET = ${PWD}/fluid.js
SOURCES = ${PWD}/gl_wrappers/gl_wrappers.c main.c
OBJECTS = gl_wrappers.o main.o


all: ${TARGET}

${TARGET}: ${OBJECTS}
	${COMPILE} ${FLAGS} -o $@ $^ ${LIBS}

${OBJECTS}: ${SOURCES}
	${COMPILE} ${FLAGS} -c $^ ${INCLUDE}

${WASM_TARGET}: ${SOURCES}
	echo "TODO!!!" > fluid.js 

clean:
	rm -f *.o ${TARGET}