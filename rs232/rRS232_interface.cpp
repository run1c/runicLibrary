#ifndef R_RS232_INTERFACE_CPP
#define R_RS232_INTERFACE_CPP

#include "rRS232_interface.h"

rRS232_interface::rRS232_interface() : rSerial_interface() {

}

rRS232_interface::rRS232_interface(const char* _dev) throw(rRS232_exception) : rSerial_interface() {
	__devName = _dev;
	init( __devName.c_str() );
}

rRS232_interface::~rRS232_interface(){
}

int rRS232_interface::sendByte(const uint8_t _byte) throw(rRS232_exception){
	return 0;
}

int rRS232_interface::sendOut(const uint8_t *_data, int _len) throw(rRS232_exception){
	return 0;
}

int rRS232_interface::readByte(uint8_t *_byte, int _timeout_ms) throw(rRS232_exception){
	return 0;
}

int rRS232_interface::readIn(uint8_t *_data, int _maxLen, int _timeout_ms) throw(rRS232_exception){
	return 0;
}

/*
 *	Private methods
 */

void rRS232_interface::init(const char* _dev){

}

#endif
