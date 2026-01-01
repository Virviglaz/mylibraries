BUILDDIR			?= build
TARGET				?= test

SRC					:= test.cpp

SRC					+= hash_db_test.cpp
SRC					+= ../hash_db.cpp

SRC					+= bitops_test.cpp
SRC					+= ../bitops.c

SRC					+= state_machine_test.cpp
SRC					+= state_machine_test.c
SRC					+= ../state_machine.c
SRC					+= filters_test.cpp

SRC					+= ../arg_parser.c
SRC					+= arg_parser_test.c

SRC					+= ../file_ops.cpp
SRC					+= file_ops_test.cpp

INC					:= .
INC					+= ../

CFLAGS				+=
CPPFLAGS			+= -std=c++14 -O0

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Running test application..."
	@./$(BUILDDIR)/$(TARGET)