SHELL = /bin/bash

COMPILE = clang++
FLAGS = -O0 -Wall -g -std=c++11
# COMPILE = c++
# FLAGS = -Og -Wall -g

INCLUDE = -I${PWD}/gl_wrappers2d -I${PWD}/imgui
LIBS = -lm -lGL -lGLEW -lglfw

TARGETDIR = .

TARGET = ${TARGETDIR}/fluid2d

IMGUI_SOURCES = ./imgui/imgui.cpp ./imgui/imgui_draw.cpp \
./imgui/imgui_impl_glfw.cpp ./imgui/imgui_impl_opengl3.cpp ./imgui/imgui_tables.cpp \
./imgui/imgui_widgets.cpp
IMGUI_OBJECTS = imgui.o imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o \
imgui_tables.o imgui_widgets.o
IMGUI_URL = https://raw.githubusercontent.com/ocornut/imgui/master

SOURCES = fluid2d.cpp ./gl_wrappers2d/gl_wrappers.cpp
OBJECTS = fluid2d.o gl_wrappers.o ${IMGUI_OBJECTS}

WEB_TARGET = ${TARGETDIR}/fluid.js
MERGE = main.cpp
SCRIPT = make_shaders_h.py

all: ${TARGET} # ${WEB_TARGET}

${TARGET}: ${OBJECTS}
	${COMPILE} ${FLAGS} -o $@ $^ ${LIBS}

${IMGUI_SOURCES}:
	curl "${IMGUI_URL}/imconfig.h" > imconfig.h
	curl "${IMGUI_URL}/imgui.h" > imgui.h
	curl "${IMGUI_URL}/backends/imgui_impl_glfw.h" > imgui_impl_glfw.h
	curl "${IMGUI_URL}/backends/imgui_impl_opengl3.h" > imgui_impl_opengl3.h
	curl "${IMGUI_URL}/imgui_internal.h" > imgui_internal.h
	curl "${IMGUI_URL}/imstb_rectpack.h" > imstb_rectpack.h
	curl "${IMGUI_URL}/imstb_textedit.h" > imstb_textedit.h
	curl "${IMGUI_URL}/imstb_truetype.h" > imstb_truetype.h
	curl "${IMGUI_URL}/imgui.cpp" > imgui.cpp
	curl "${IMGUI_URL}/imgui_draw.cpp" > imgui_draw.cpp
	curl "${IMGUI_URL}/backends/imgui_impl_glfw.cpp" > imgui_impl_glfw.cpp
	curl "${IMGUI_URL}/backends/imgui_impl_opengl3.cpp" > imgui_impl_opengl3.cpp
	curl "${IMGUI_URL}/imgui_tables.cpp" > imgui_tables.cpp
	curl "${IMGUI_URL}/imgui_widgets.cpp" > imgui_widgets.cpp
	mkdir ./imgui
	mv imgui*.cpp imgui*.h imconfig.h imstb*.h ./imgui

${OBJECTS}: ${SOURCES} ${IMGUI_SOURCES}
	${COMPILE} ${FLAGS} -c $^ ${INCLUDE}

${WEB_TARGET}: ${SCRIPT} ${SOURCES} ${IMGUI_SOURCES}
	python3 ${SCRIPT}
	emcc -o $@ ${SOURCES} ${IMGUI_SOURCES} ${INCLUDE} -std=c++11 -O3 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1
	rm -f shaders.hpp

clean:
	rm -f *.o *~ *.wasm *.mem *.js main.cpp ${TARGET}

