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
SRCS = $(wildcard *.cpp)
# The header file search path
INC = -I./
# Dependent libraries,Select library file According to the operating system you using
OS_TYPE = $(shell uname -m)
ifeq ($(OS_TYPE), x86_64)
LIBS = -L./lib/x86_64 -lUSB2XXX -lusb-1.0 -lpthread
else
LIBS = -L./lib/x86_32 -lUSB2XXX -lusb-1.0 -lpthread
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
