#ifndef R_SERIAL_INTERFACE
#define R_SERIAL_INTERFACE

#define DEFAULT_TIMEOUT_MS	100

/*
 *	Abstract base class for serial communication interfaces
 */

#include <stdint.h>	// for uint8_t
#include <string>

class rSerial_interface {
public:
	// Base class constructor
	rSerial_interface() {};
	// Virtual destructor: derived objects can implement own destruction behaviour in addition to base destructor (which is empty in this case) 
	virtual ~rSerial_interface() {};

	// Returns the name of the interface e.g. "RS232", "USB", e.a.
	std::string getInterfaceName() { return __interfaceName; };

	// Default send methods: pure virtual mehtods, have to be implemented by derived class
	// Returns the number of bytes transferred
	virtual int sendOut(const uint8_t *_data, int _len) = 0;

	// Default receive methods with timeout: pure virtual mehtods, have to be implemented by derived class
	// Returns the number of bytes received
	virtual int readIn(uint8_t *_data, int _maxLen, int _timeout_ms = DEFAULT_TIMEOUT_MS) = 0;

protected:
	std::string __interfaceName;

};

#endif
