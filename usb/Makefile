OS=$(shell uname)

## Output object files
OBJ=rUSB_interface.o

## Subdirectories
SUBDIR=Hantek

## Compiler
CC=g++

## Compiler flags
CFLAGS=-I../ $(LIBUSB_CFLAGS) -fPIC -Wall

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
	@cd $(SUBDIR); $(MAKE) clean;
