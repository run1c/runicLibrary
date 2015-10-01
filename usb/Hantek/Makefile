## Libusb compiler flags
#LIBUSB_CFLAGS=$(shell libusb-config --cflags)

## Output object files
OBJ=HantekDSO_5000P.o

## Subdirectories
SUBDIR=

## Compiler
CC=g++

## Compiler flags
CFLAGS=-I../../ $(LIBUSB_CFLAGS) -fPIC -Wall

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
