## libusb inc dir
INC_LUSB=/opt/local/include/libusb-1.0

## Output object files
OBJ=rArgHandler.o

## Compiler
CC=g++

## Compiler flags
CFLAGS=-I../ -I$(INC_LUSB) -fPIC -Wall

.PHONY: all clean

all: $(OBJ)

%.o:    %.cpp | %.h
	@echo [MAKE] Compiling $<
	$(CC) -c $< $(CFLAGS)
	
clean:
	@echo [MAKE] Cleanup '$(shell pwd)'...
	rm -f *.o
