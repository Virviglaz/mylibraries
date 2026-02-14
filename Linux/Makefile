BUILDDIR		:= build
TARGET			:= test

INC				:= ../Common

SRC				:= test.cpp
SRC				+= i2c.cpp
SRC				+= spi.cpp

CPPFLAGS		:= -std=c++14 -O0

LDFLAGS			:= -rdynamic

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Build complete: $(BUILDDIR)/$(TARGET)"
	@echo "Run test:"
	@./$(BUILDDIR)/$(TARGET)