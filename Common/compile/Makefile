BUILDDIR			?= build
TARGET				?= compile_only

SRC					+= $(shell find .. -name '*.cpp')
SRC					+= $(shell find .. -name '*.c')

INC					:= .
INC					+= ../

CFLAGS				+= -pedantic
CPPFLAGS			+= -std=c++14 -O2

include Makefile.common