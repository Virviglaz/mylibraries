# BuildServices
Generic makefiles to build linux/embedded applications and libraries

Example of use: create Makefile and add following lines:

```
BUILDDIR		:= build
TARGET			:= test

INC				:= # list of folders to include

SRC				:= test.cpp # source files
SRC				+= i2c.cpp
SRC				+= spi.cpp

CPPFLAGS		:= -std=c++14 -O0 # extra CPP flags if needed

LDFLAGS			:= -rdynamic

include Makefile.common
```
Compile and run your application:
```
$ make run
```
