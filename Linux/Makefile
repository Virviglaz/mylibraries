BUILDDIR		:= build
TARGET			:= test

SRC				:= test.cpp

CPPFLAGS		:= -std=c++14 -O0

LDFLAGS			:=

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Build complete: $(BUILDDIR)/$(TARGET)"
	@echo "Run test:"
	@./$(BUILDDIR)/$(TARGET)