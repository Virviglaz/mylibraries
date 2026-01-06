BUILDDIR		:= build
TARGET			:= test_server

SRC				:= test.cpp
SRC				+= server.cpp

CPPFLAGS		:= -std=c++14 -O0

LDFLAGS			:= -lpthread

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Build complete: $(BUILDDIR)/$(TARGET)"
	@echo "Run test:"
	@./$(BUILDDIR)/$(TARGET)