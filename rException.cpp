#ifndef R_EXCEPTION_CPP
#define R_EXCEPTION_CPP

#include "rException.h"
#include <stdio.h>	// for sprintf

rException::rException(std::string _newMessage,int _errNumber) : exception(), __message(_newMessage), __errNo(_errNumber) {
/*	char _buf[20];
	sprintf(_buf, " (Error Number %i)", __errNo);
	__message += _buf;
*/
};
	
const char* rException::what() const throw() { 
	return __message.c_str(); 
};	

int rException::getErrorNumber() throw() { 
	return __errNo; 
};

#endif
