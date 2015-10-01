#ifndef R_RS232_INTERFACE
#define R_RS232_INTERFACE

#define DEFAULT_TIMEOUT_MS	100

/*
 *	Exception for RS232 interfaces
 */

#include <rException.h>

class rRS232_exception : public rException {

public:
	rRS232_exception(std::string _message, int _errNumber = 0) : rException("[rRS232_exception] - " + _message, _errNumber) {};
	~rRS232_exception() throw () {};

};


/*
 *	Class for serial communication using RS232 	
 */

#include <rSerial_interface.h>
#include <string>

/* RS232 data format */
#define R_RS232_PAR_ODD		0
#define R_RS232_PAR_EVEN	1

class rRS232_interface : public rSerial_interface {

public:
	rRS232_interface();
	// Open given device file
	rRS232_interface(const char* _dev) throw(rRS232_exception);
	virtual ~rRS232_interface();

	// Set baud rate
	void setBaud(int _baud) throw(rRS232_exception);

	// Set parity to even/odd
	void setParity(int _par) throw(rRS232_exception);

	// Set number of stop bits
	void setStopBits(int _nStopBits) throw(rRS232_exception);

	// Set number of bits per transfer
	void setBits(int _nBits) throw(rRS232_exception);

	// Default send methods
	// Returns the number of bytes transferred
	int sendByte(const uint8_t _byte) throw(rRS232_exception);
	int sendOut(const uint8_t *_data, int _len) throw(rRS232_exception);

	// Default receive methods
	// Returns the number of bytes received
	int readByte(uint8_t *_byte, int _timeout_ms = DEFAULT_TIMEOUT_MS) throw(rRS232_exception);
	int readIn(uint8_t *_data, int _maxLen, int _timeout_ms = DEFAULT_TIMEOUT_MS) throw(rRS232_exception);

private:
	std::string __devName;
	int __fd, __baud, __parity, __nStopBits, __nBits;

	// Initialization process
	void init(const char* _dev);

};

#endif
