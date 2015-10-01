OS=$(shell uname)

## Libusb compiler flags
#LIBUSB_CFLAGS=$(shell libusb-config --cflags)

## Output object files
OBJ=HantekDSO_5000P.o

## Subdirectories
SUBDIR=

## Compiler
CC=g++

## Compiler flags
CFLAGS=-I../../ -fPIC -Wall

ifeq ($(OS),Darwin)
  CFLAGS+=$(shell libusb-config --cflags)
endif

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
