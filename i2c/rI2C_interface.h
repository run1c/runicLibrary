#ifndef R_I2C_INTERFACE
#define R_I2C_INTERFACE

#define DEFAULT_TIMEOUT_MS	100

/*
 *	Exception for I2C interfaces
 */

#include <rException.h>

class rI2C_exception : public rException {

public:
	rI2C_exception(std::string _message, int _errNumber = 0) : rException("[rI2C_exception] - " + _message, _errNumber) {};
	~rI2C_exception() throw () {};

};


/*
 *	Class for serial communication using I2C 	
 */

#include <rSerial_interface.h>
#include <string>

/* I2C data format */
#define R_I2C_PAR_ODD		0
#define R_I2C_PAR_EVEN	1

class rI2C_interface : public rSerial_interface {

public:
	rI2C_interface();
	// Open given device file
	rI2C_interface(const char* _dev) throw(rI2C_exception);
	virtual ~rI2C_interface();

	// Claim bus and set slave address
	void setAddress(uint8_t _addr) throw(rI2C_exception);

	// Default send methods
	// Returns the number of bytes transferred
	int sendByte(const uint8_t _byte) throw(rI2C_exception);
	int sendOut(const uint8_t *_data, int _len) throw(rI2C_exception);
	// Writing to a register with given address
	int writeReg(uint8_t _reg_addr, const uint8_t *_data, int _len) throw(rI2C_exception);

	// Default receive methods
	// Returns the number of bytes received
	int readByte(uint8_t *_byte, int _timeout_ms = DEFAULT_TIMEOUT_MS) throw(rI2C_exception);
	int readIn(uint8_t *_data, int _maxLen, int _timeout_ms = DEFAULT_TIMEOUT_MS) throw(rI2C_exception);
	// Reading from a register with given address
	int readReg(uint8_t _reg_addr, uint8_t *_data, int _maxLen, int _timeout_ms = DEFAULT_TIMEOUT_MS) throw(rI2C_exception);

private:
	uint8_t __curSlaveAddr;
	std::string __devName;
	int __fd;
	unsigned long __i2c_funcs;
	bool __smBusOnly, __10bitAddr;

	// Initialization process
	void init(const char* _dev);

};

#endif
