CFLAGS          += -Wall -Wno-format-truncation
ARCH		?= $(shell arch)
OBJ             := $(subst .c,.o,$(shell ls *.c))
TRG             ?= lib_$(ARCH)
CC		?= gcc

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INC) $(DEF) -fpic

all: $(OBJ) 
	$(CC) -shared -o $(TRG).so  $(OBJ)

clean:
	@rm -f *.o

