CFLAGS          += -Wall -Wno-format-truncation
ARCH		?= $(shell arch)
OBJ             := $(subst .c,.o,$(shell ls *.c))
TRG             ?= lib_$(ARCH)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) $(INC) $(DEF)

%.a: %.o
	ar rcs $@ $^

all: $(OBJ) 
	ar rcs $(TRG).a  $(OBJ)

clean:
	@rm -f *.o

