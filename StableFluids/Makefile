SHELL = /bin/bash
FLAGS = -O0 -Wall -g
COMPILE = clang


ifdef TERM_PROGRAM
ifeq ($(TERM_PROGRAM),Apple_Terminal)
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers -I/opt/homebrew/include
LIBS = -ldl -L/opt/homebrew/lib -lglfw\
       -framework CoreVideo -framework OpenGL -framework IOKit\
       -framework Cocoa -framework Carbon
endif
else
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers
LIBS = -lm -lGL -lGLEW -lglfw
endif

WEB_INCLUDE = ${INCLUDE} -I${HOME}/emsdk/upstream/emscripten/system/include  

TARGET = ${PWD}/program
WEB_TARGET = main.js
SOURCES = ${PWD}/gl_wrappers/gl_wrappers.c simulation.c main.c
OBJECTS = gl_wrappers.o simulation.o main.o


all: ${TARGET}

${TARGET}: ${OBJECTS}
	${COMPILE} ${FLAGS} -o $@ $^ ${LIBS}

${WEB_TARGET}: ${SOURCES}
	EMCC_DEBUG=1
	emcc -o $@ $^ ${INCLUDE} -v -s WASM=1 -s USE_GLFW=3 -s FULL_ES3=1 \
    -s TOTAL_MEMORY=100MB --embed-file shaders

${OBJECTS}: ${SOURCES}	
	${COMPILE} ${FLAGS} -c $^ ${INCLUDE}

clean:
	rm -f *.o *.js *.wasm ${TARGET}
