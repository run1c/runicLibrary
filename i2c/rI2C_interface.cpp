#ifndef R_I2C_INTERFACE_CPP
#define R_I2C_INTERFACE_CPP

#include "rI2C_interface.h"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

rI2C_interface::rI2C_interface() : rSerial_interface() {

}

rI2C_interface::rI2C_interface(const char* _dev) throw(rI2C_exception) : rSerial_interface(), __curSlaveAddr(0xFF) {
	__devName = _dev;

	// Open device file
	__fd = open(__devName.c_str(), O_RDWR);
	if (__fd < 0)
		throw( rI2C_exception("constructor : Failed to open file descriptor.", __fd) );

	// Gather information about I2C controller
	int _ret = ioctl(__fd, I2C_FUNCS, &__i2c_funcs);
	if (_ret < 0)
		throw( rI2C_exception("constructor : Failed to gather controller function information.", _ret) );

	// Extract information
	(__i2c_funcs & I2C_FUNC_I2C) ? __smBusOnly = false : __smBusOnly = true;
	(__i2c_funcs & I2C_FUNC_10BIT_ADDR) ? __10bitAddr = true : __10bitAddr = false;

	// Done!	

}

rI2C_interface::~rI2C_interface(){
	// Close device file
	if (__fd >= 0) close(__fd);
}

void rI2C_interface::setAddress(uint8_t _addr) throw(rI2C_exception){
	// Only act if slave address has been changed
	if (_addr == __curSlaveAddr)
		return;
	// Set address and claim bus
	int _ret = ioctl(__fd, I2C_SLAVE, _addr);
	if (_ret < 0)
		throw( rI2C_exception("setAddress : Bus arbitration failed.", _ret) );
	// Update current slave address
	__curSlaveAddr = _addr;		

	return;
}

int rI2C_interface::sendByte(const uint8_t _byte) throw(rI2C_exception){
	uint8_t _buf[1];
	_buf[0] = _byte;
	return sendOut(_buf, 1);
}

int rI2C_interface::sendOut(const uint8_t *_data, int _len) throw(rI2C_exception){
	int _ret = write(__fd, _data, _len);
	if (_ret < 0)
		throw( rI2C_exception("sendOut : Write failed.", _ret) );
	else if (_ret != _len)
		throw( rI2C_exception("sendOut : Wrong number of bytes sent.", _ret) );

	// Everything went fine, return number of bytes successfully written
	return _ret;
}

int rI2C_interface::writeReg(uint8_t _reg_addr, const uint8_t *_data, int _len) throw(rI2C_exception) {
	// Add address as the first transmitted byte
	uint8_t* _buf = new uint8_t[_len + 1];
	_buf[0] = _reg_addr;
	for (int i = 0; i < _len; i++)
		_buf[i + 1] = _data[i];
	return sendOut(_buf, _len + 1); 
}


int rI2C_interface::readByte(uint8_t *_byte, int _timeout_ms) throw(rI2C_exception){
	return readIn(_byte, 1);
}

int rI2C_interface::readIn(uint8_t *_data, int _len, int _timeout_ms) throw(rI2C_exception){
	int _ret = read(__fd, _data, _len);
	if (_ret < 0)
		throw( rI2C_exception("readIn : Read failed.", _ret) );
	else if (_ret != _len)
		throw( rI2C_exception("readIn : Wrong number of bytes read.", _ret) );

	return _ret;
}

int rI2C_interface::readReg(uint8_t _reg_addr, uint8_t *_data, int _maxLen, int _timeout_ms) throw(rI2C_exception) {
	// First set the register by sending the address byte to the slave
	sendByte(_reg_addr);
	// Register is addressed, read content
	return readIn(_data, _maxLen);
}



/*
 *	Private methods
 */

void rI2C_interface::init(const char* _dev){

}

#endif
