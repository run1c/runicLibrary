## List of all bus systems
BUS=utils
BUS+=usb
BUS+=rs232

## Object files in root directory
OBJ+=rException.o

## Runic software library
## This labrary has to be linked together with libusb!
LIBNAME=libRunic.a

## Compiler 
CC=g++

## Compiler flags
CFLAGS=-I./ -fPIC -Wall 

.PHONY: $(BUS)

all:	$(OBJ) $(BUS)

$(BUS):
	@echo [MAKE] Compiling '$@'
	@cd $@;	$(MAKE);
	
%.o:    %.cpp | %.h
	@echo [MAKE] Compiling '$<'
	$(CC) -c $< $(CFLAGS)	

lib:	all
	@rm -f $(LIBNAME)
	@ar rsc $(LIBNAME) `for dir in $(BUS); do ( find $$dir -name '*.o'; ); done` `ls *.o`
	@echo "\n[MAKE] $(LIBNAME) contains the following objects:"
	@ar tv $(LIBNAME)

libso:	all
	@echo -n " LIB:\t"
	g++ -shared -fPIC -o liblab.so `for dir in $(BUS); do ( find $$dir -name '*.o'; ); done` `ls *.o`

clean:
	@echo [MAKE] Cleanup ...
	rm -f $(LIBNAME)
	rm -f $(LIBNAME:.a=.so)
	rm -f $(OBJ)
	@for dir in $(BUS); do ( cd $$dir; $(MAKE) clean; ); done
