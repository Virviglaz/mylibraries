BUILDDIR			?= build
TARGET				?= test

SRC					:= test.cpp
SRC					+= hash_db_test.cpp
SRC					+= ../hash_db.cpp

INC					:= .
INC					+= ../

CFLAGS				+=
CPPFLAGS			+= -std=c++14 -O2

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Running test application..."
	@./$(BUILDDIR)/$(TARGET)