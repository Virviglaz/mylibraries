BUILDDIR			?= build
TARGET				?= test

SRC					:= test.cpp

# Hash DB tests
SRC					+= hash_db_test.cpp
SRC					+= ../hash_db.cpp

# Bit operations tests
SRC					+= bitops_test.cpp
SRC					+= ../bitops.c

# State machine tests
SRC					+= state_machine_test.cpp
SRC					+= state_machine_test.c
SRC					+= ../state_machine.c
SRC					+= ../state_machine.cpp
SRC					+= filters_test.cpp

# Arg parser tests
SRC					+= ../arg_parser.c
SRC					+= arg_parser_test.c

# File operations tests
SRC					+= ../file_ops.cpp
SRC					+= file_ops_test.cpp

# Devices JSON tests
SRC					+= devices_json_test.cpp
SRC					+= ../devices_json.cpp

# DS18B20 tests
SRC					+= DS18B20_json_test.cpp
SRC					+= ../DS18B20.cpp
SRC					+= ../DS18B20_json.cpp
SRC					+= ../crc.c

INC					:= .
INC					+= ../

CFLAGS				+=
CPPFLAGS			+= -std=c++14 -O0

LDFLAGS				+= -ljsoncpp

include Makefile.common

test: $(BUILDDIR)/$(TARGET)
	@echo "Running test application..."
	@./$(BUILDDIR)/$(TARGET)

install_json:
	sudo apt-get install libjsoncpp-dev
