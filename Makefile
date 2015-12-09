#
# Makefile for ppgso examples
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

CC = g++
CFLAGS = -g -O0 -std=c++11 -Wextra -Wformat=0 #-Wall
LFLAGS = -lm
UNAME := $(shell uname -s)

ALL = gl_project

all: $(ALL)

gl_%: gl_%.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS) $(GL_LFLAGS)

GL_LFLAGS = -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread
clean:
	-rm $(ALL)
