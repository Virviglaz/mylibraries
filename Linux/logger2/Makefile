BUILDDIR			?= build
TARGET				?= logger2

SRC					:= logger2.cpp
SRC					+= file_ops.cpp
SRC					+= server.cpp

INC					:= ../../Common
INC					+= ../server
LDFLAGS				+= -ljsoncpp

include ../../BuildServices/Makefile.common

install_deps: install_json

install_json:
	sudo apt-get install libjsoncpp-dev

test: install_deps
	$(MAKE) all
	$(MAKE) -C test all
	@./$(BUILDDIR)/$(TARGET) --test &
	$(MAKE) -C test run
#	rm -f /tmp/logger1.txt
#	rm -f /tmp/logger2.txt