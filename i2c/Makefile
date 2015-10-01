## Output object files
# 	i2c-dev does not exist under Mac OS
#	so it will only compile on Linux

OS=$(shell uname)
ifeq ($(OS),Linux)
  OBJ=rI2C_interface.o
endif
ifeq ($(OS),Darwin)
  OBJ=
endif

## Subdirectories
SUBDIR=

## Compiler
CC=g++

## Compiler flags
CFLAGS=-I../ -fPIC -Wall

.PHONY: all clean $(SUBDIR)

all: 	$(OBJ) $(SUBDIR)

$(SUBDIR):
	@echo [MAKE] Compiling subdir $@
	@cd $@;	$(MAKE);

%.o:    %.cpp | %.h
	@echo [MAKE] Compiling $<
	$(CC) -c $< $(CFLAGS)
	
clean:
	@echo [MAKE] Cleanup '$(shell pwd)'...
	rm -f *.o
#	@cd $(SUBDIR); $(MAKE) clean;
