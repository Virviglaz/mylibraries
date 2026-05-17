BUILDDIR			?= build
TARGET				?= compile_only

SRC					+= $(shell find .. -maxdepth 1 -name '*.cpp' )
SRC					+= $(shell find .. -maxdepth 1 -name '*.c' )

INC					:= .
INC					+= ../

CFLAGS				+= -pedantic
CPPFLAGS			+= -std=c++17 -O2

LDFLAGS				+= -ljsoncpp

include Makefile.common