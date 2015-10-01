CC=g++

CFLAGS=-Wall -fPIC -I../ $(shell libusb-config --cflags)
LDFLAGS=$(shell libusb-config --libs) -L../ -lRunic -lusb-1.0

BIN=hantek_example
OBJ=$(BIN).o

.PHONY: clean

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN)
	rm -rf *.o
