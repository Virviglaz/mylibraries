BUILDDIR		:= build
TARGET			:= test

INC				:=../Linux
SRC				:= gmp_pi_test.cpp

CPPFLAGS		:= -std=c++14 -O0

LDFLAGS			:= -rdynamic
LDFLAGS			+= -lgmp

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Build complete: $(BUILDDIR)/$(TARGET)"
	@echo "Run test:"
	@./$(BUILDDIR)/$(TARGET)