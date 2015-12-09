#
# Makefile for ppgso examples
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

CC = g++
CFLAGS = -O2 -std=c++11 -Wpedantic -Wall -Wfloat-equal -Wextra -Wsign-promo -Wsign-compare -Wconversion -Wno-sign-conversion -I/usr/local/include -L/usr/local/lib
LFLAGS = -lm
UNAME := $(shell uname -s)

ALL = gl_project

all: $(ALL)

gl_%: gl_%.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LFLAGS) $(GL_LFLAGS)

# Linux
ifeq ($(UNAME),Linux)
GL_LFLAGS = -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread
clean:
	-rm $(ALL)

# OSX
else ifeq ($(UNAME),Darwin)
GL_LFLAGS = -lGLEW -lglfw3 -framework OpenGL
clean:
	-rm $(ALL)

# Windows
else
GL_LFLAGS = -lglew32 -lglfw3 -lOpenGL32 -lglu32
clean:
	-del *.exe

endif
