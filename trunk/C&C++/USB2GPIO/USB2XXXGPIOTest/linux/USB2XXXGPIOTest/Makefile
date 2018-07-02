# Instructions to the compiler and options
CC = g++
RM = rm
CFLAGS = -Wall
# Macro definition
DEFS = -D OS_UNIX
CFLAGS += $(DEFS)
# The target file  
TARGET = $(notdir $(CURDIR))
# The source file
DIR_SRC = ../../source
SRCS = $(wildcard ${DIR_SRC}/*.cpp)
# The header file search path
INC = -I../../source
# Dependent libraries,Select library file According to the operating system you using
OS_TYPE = $(shell uname -m)
ifeq ($(OS_TYPE), x86_64)
LIBS = -L./lib/x64 -lUSB2XXX -lusb-1.0 -lpthread 
endif
ifeq ($(OS_TYPE), x86_32)
LIBS = -L./lib/x86 -lUSB2XXX -lusb-1.0 -lpthread
endif
ifeq ($(findstring armv6,$(OS_TYPE)), armv6)
LIBS = -L./lib/ARMv7 -lUSB2XXX -lusb-1.0 -lpthread
endif
ifeq ($(findstring armv7,$(OS_TYPE)), armv7)
LIBS = -L./lib/ARMv7 -lUSB2XXX -lusb-1.0 -lpthread
endif
ifeq ($(findstring armv8,$(OS_TYPE)), armv8)
LIBS = -L./lib/ARMv7 -lUSB2XXX -lusb-1.0 -lpthread
endif
# The target file
OBJS = $(SRCS:.cpp=.o)
# Link for the executable file
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIBS)
	$(RM) -rf $(OBJS) *~
clean:
	$(RM) -rf $(TARGET) $(OBJS) *~

%.o:%.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
