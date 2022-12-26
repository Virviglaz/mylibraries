CFLAGS          += -Wall -Wno-format-truncation
INC             +=
DEF             ?=
CC              ?= gcc
OBJ             := $(subst .c,.o,$(shell ls *.c))
TRG             ?= $(notdir $(PWD))
LIB		?= -lpthread

# Compile C files
%.o: %.c
	@echo 'Compiling $@...'
	$(CC) -c -o $@ $< $(CFLAGS) $(INC) $(DEF)

all: $(OBJ)
	$(CC) $(OBJ) -o $(TRG) $(CFLAGS) $(LIB)

test: clean all

clean:
	rm -f $(OBJ) $(TRG)

