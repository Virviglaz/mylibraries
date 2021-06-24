CFLAGS	+= -Wall
INC		+=
DEF		+=
LIB		+= -lpthread
CC		?= gcc
OBJ		:= $(subst .c,.o,$(shell ls *.c))
TRG		?= $(notdir $(PWD)) 

# Compile C files
%.o: %.c
	@echo 'Compiling $@...'
	$(CC) -c -o $@ $< $(CFLAGS) $(INC) $(DEF) $(LIB)

all:  $(OBJ)
	$(CC) $(OBJ) -o $(TRG) $(CFLAGS) $(LIB)

clean:
	rm -f $(OBJ) $(TRG)