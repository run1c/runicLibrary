## Output object files
OBJ=rRS232_interface.o

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
