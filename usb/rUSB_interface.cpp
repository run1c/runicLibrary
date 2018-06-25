#ifndef R_USB_INTERFACE_CPP
#define R_USB_INTERFACE_CPP

#include "rUSB_interface.h"

//#define DEBUG

// Default context
libusb_context* rUSB_interface::__context = NULL;
int rUSB_interface::__device_count = 0;

#include <stdlib.h>
#include <stdio.h>

rUSB_interface::rUSB_interface(){

};

rUSB_interface::rUSB_interface(uint16_t _vid, uint16_t _pid) throw(rUSB_exception) : rSerial_interface() {
	int _ret = -1;
	libusb_device_handle* _tmpHandle;

	// if this is the first device, start libusb session
	if (__device_count == 0){ 
		_ret = libusb_init(&__context);
		if (_ret < 0)
			throw( rUSB_exception("rUSB_interface constructor : error on starting libusb session.", _ret) );
	}

	// Open device and create handle	
	_tmpHandle = libusb_open_device_with_vid_pid(__context, _vid, _pid);
	if (_tmpHandle == NULL)
		throw( rUSB_exception("rUSB_interface constructor : cannot open device.") );

	/*
	 *	Use handle to gather device information and I/O control	
	 */

	init(_tmpHandle);

	// After init vid/pid are received from the device, check if they match
	if ( (_vid != __vid) || (_pid != __pid) )
		throw( rUSB_exception("rUSB_interface constructor : vid and pid do not match.") );
	
	// Done!
}

rUSB_interface::rUSB_interface(libusb_device_handle *_handle) throw(rUSB_exception){
	if (__context == NULL)
		throw ( rUSB_exception("rUSB_exception constructor : no context defined for handle.") );
	init(_handle);
}

rUSB_interface::~rUSB_interface(){
	// Release current interface
	if (__curInterfaceNo != RUSB_NO_INTERFACE)
		libusb_release_interface(__handle, __curInterfaceNo);
	// Close device if there is something to be closed
	if (__handle != NULL)
		libusb_close(__handle);
	// If this is the last interface of the session, close the session aswell
	if (__device_count == 1){
		libusb_exit(NULL);
	}
	// Decrement device count
	__device_count--;
}

int rUSB_interface::sendOut(const uint8_t *_data, int _len) throw(rUSB_exception){
	return 0;
}

int rUSB_interface::readIn(uint8_t *_data, int _maxLen, int _timeout_ms) throw(rUSB_exception){
	return 0;
}

void rUSB_interface::claimInterface(int _interfaceNo) throw(rUSB_exception){
	int _ret = 0;
	if (__curInterfaceNo != _interfaceNo){
		// Release the old interface (if there is one) before claiming a new one
		if (__curInterfaceNo != RUSB_NO_INTERFACE){ 
			_ret = libusb_release_interface(__handle, 0);
			if (_ret < 0)
				throw ( rUSB_exception("claimInterface : cannot release old interface.", _ret) );
		}

		// Detach any kernel modules from the interface, if one is active
		if (libusb_kernel_driver_active(__handle, _interfaceNo) == 1) {
			if ( libusb_detach_kernel_driver(__handle, _interfaceNo) )
				throw ( rUSB_exception("claimInterface : cannot detach kernel module from interface.", _ret) );
		}

		// Claim new interface (if told to do so)
		if (_interfaceNo != RUSB_NO_INTERFACE){ 
			_ret = libusb_claim_interface(__handle, 0);
			if (_ret < 0)
				throw ( rUSB_exception("claimInterface : cannot claim new interface.", _ret) );
		}
		__curInterfaceNo = _interfaceNo;
	}
	
	return;
}

int rUSB_interface::controlTransfer(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, uint8_t _endpoint, int _maxTries) throw(rUSB_exception){
	int _errorCode = LIBUSB_ERROR_TIMEOUT;

	// Retry transmission if a timeout error was received
//	for (int _iTry = 0; (_iTry < _maxTries) && (_errorCode == LIBUSB_ERROR_TIMEOUT); _iTry++ )
	_errorCode = libusb_control_transfer( __handle, _endpoint, _request, _index, _value, _buffer, _len, RUSB_DEFAULT_TIMEOUT_MS ); 

	// Error handling
	if (_errorCode < 0)
		throw( rUSB_exception("controlTransfer : error on transmission.", _errorCode) );

	// Return bytes transferred
	return _errorCode;
}

int rUSB_interface::readInControl(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, int _maxTries) throw(rUSB_exception){
	return controlTransfer(_request, _index, _value, _buffer, _len, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, _maxTries);
}

int rUSB_interface::readInControl(uint8_t _request, unsigned char* _buffer, int _len, int _maxTries) throw(rUSB_exception){
	return readInControl(_request, 0, 0, _buffer, _len, _maxTries);
}

int rUSB_interface::sendOutControl(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, int _maxTries) throw(rUSB_exception){
	return controlTransfer(_request, _index, _value, _buffer, _len, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT, _maxTries);
}

int rUSB_interface::sendOutControl(uint8_t _request, unsigned char* _buffer, int _len, int _maxTries) throw(rUSB_exception){
	return sendOutControl(_request, 0, 0, _buffer, _len, _maxTries);
}

int rUSB_interface::bulkTransfer(unsigned char* _data, int _len, uint8_t _endpoint, int _maxTries) throw(rUSB_exception){
	if (__curInterfaceNo == RUSB_NO_INTERFACE)
		throw( rUSB_exception("bulkTransfer : no interface claimed.") );
	int _bytesTransferred = -1, _errorCode = LIBUSB_ERROR_TIMEOUT;

	// Retry transmission if a timeout error was received
//	for (int _iTry = 0; (_iTry < _maxTries) && (_errorCode == LIBUSB_ERROR_TIMEOUT); _iTry++ )
	_errorCode = libusb_bulk_transfer( __handle, _endpoint, _data, _len, &_bytesTransferred, RUSB_DEFAULT_TIMEOUT_MS);

	// Error handling
	if (_errorCode < 0)
		throw( rUSB_exception("bulkTransfer : error on transmission", _errorCode) );

	// Return bytes transferred
	return _bytesTransferred;
}

int rUSB_interface::sendOutBulk(unsigned char* _data, int _len, uint8_t _endpoint, int _maxTries) throw(rUSB_exception){
	int _bytesSent = 0;
	try {
		_bytesSent = bulkTransfer(_data, _len, LIBUSB_ENDPOINT_OUT | _endpoint, _maxTries);
	} catch (rUSB_exception &ex) {
		// Rethrow exception with additional info
		throw ( rUSB_exception(std::string("sendOutBulk : ") + ex.what(), ex.getErrorNumber()) );
	}

	return _bytesSent;
}

int rUSB_interface::readInBulk(unsigned char* _data, int _len, uint8_t _endpoint, int _maxTries) throw(rUSB_exception){
	int _bytesReceived = 0;
	try {
		_bytesReceived = bulkTransfer(_data, _len, LIBUSB_ENDPOINT_IN | _endpoint, _maxTries);
	} catch (rUSB_exception &ex) {
		// Rethrow exception with additional info
		throw ( rUSB_exception(std::string("readInBulk : ") + ex.what(), ex.getErrorNumber()) );
	}

	return _bytesReceived;
}

std::string rUSB_interface::getManufacturerName(){
	return __manufacturerName;
}

std::string rUSB_interface::getProductName(){
	return __productName;
}

int rUSB_interface::getSerialNumber(){
	return __serialNo;
}
 
int rUSB_interface::getBusNumber(){
	return __busNumber;
}

int rUSB_interface::getDevAddress(){
	return __devAddress;
}

uint16_t rUSB_interface::getVendorID(){
	return __vid;
}

uint16_t rUSB_interface::getProductID(){
	return __pid;
}

struct rUSB_endpoint rUSB_interface::getEndpointInfo(int _iEndpoint){
	return __endpoints.at(_iEndpoint);
}

int rUSB_interface::getNInterfaces(){
	return __nInterfaces;
} 

int rUSB_interface::getNEndpoints(){
	return __nEndpoints;
} 

void rUSB_interface::printInfo(){

	printf("VID:PID 0x%04X:0x%04X ", __vid, __pid);
	printf("- '%s, %s'\n", __manufacturerName.c_str(), __productName.c_str() );
	printf("Serial no.: %i - no. of interfaces: %i - no. of endpoints: %i\n", __serialNo, __nInterfaces, __nEndpoints);

	std::string _ep_type; 

	for (int _iEndpoint = 0; _iEndpoint < __nEndpoints; _iEndpoint++){

		switch (__endpoints.at(_iEndpoint).type){
		case LIBUSB_TRANSFER_TYPE_CONTROL:
			_ep_type = "CONTROL";
			break;
		case LIBUSB_TRANSFER_TYPE_BULK:
			_ep_type = "BULK";
			break;
		case LIBUSB_TRANSFER_TYPE_INTERRUPT:
			_ep_type = "INTERRUPT";
			break;
		case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
			_ep_type = "ISOCHRONOUS";
			break;
		}

		printf("EP%i:\tAddress: 0x%02X\n", _iEndpoint+1, __endpoints.at(_iEndpoint).address);
		printf("\tType: %s\n", _ep_type.c_str());
		printf("\tDir: %s\n", (__endpoints.at(_iEndpoint).isOUT) ? "OUT" : "IN");
		printf("\tInterface no.: %i\n", __endpoints.at(_iEndpoint).interfaceNo);
		printf("\tSetting no.: %i\n", __endpoints.at(_iEndpoint).settingNo);
	}	
	printf("\n");
	return;
}

libusb_context* rUSB_interface::getContext() throw(rUSB_exception) {
	if (__context == NULL)	// throw exception if no context is opened
		throw ( rUSB_exception("getContext : no context opened.") );
	return __context;
}

void rUSB_interface::setContext(libusb_context *_context) throw(rUSB_exception) {
	if (_context == NULL)	// throw exception if no context is opened
		throw ( rUSB_exception("setContext : no context provided.") );
	__context = _context;
}

int rUSB_interface::getDevCount(){
	return __device_count;
}

/*
 *	P R I V A T E   M E T H O D S
 */

void rUSB_interface::init(libusb_device_handle *_handle) throw(rUSB_exception) {
	int _ret = -1;
	char _cBuf[RUSB_BUF_LEN];
	libusb_device_descriptor _desc;
	__curInterfaceNo = RUSB_NO_INTERFACE;	// Select EP0	
	__interfaceName = "USB";

	// Save handle	
	__handle = _handle;

	// Get device 
	__device = libusb_get_device(__handle);
	if (__device == NULL)
		throw( rUSB_exception("rUSB_interface constructor : error on libusb_get_device().") );

	// Use device to get some information	
	_ret = libusb_get_device_descriptor(__device, &_desc);
	if (_ret < 0)
		throw( rUSB_exception("rUSB_interface constructor : error on retrieving device descriptor.", _ret) );

	// Get vid and pid
	__vid = _desc.idVendor;
	__pid = _desc.idProduct;

	// Get bus number and device address on bus
	__busNumber = libusb_get_bus_number(__device);
	__devAddress = libusb_get_device_address(__device);

	// Get manufacturer and product name if specified
	if ( _desc.iManufacturer != 0 ){
		_ret = libusb_get_string_descriptor_ascii( __handle, _desc.iManufacturer, (unsigned char *)_cBuf, sizeof(_cBuf) );
		if (_ret < 0)
			throw( rUSB_exception("rUSB_interface constructor : error on retrieving string descriptor of manufacturer ID.", _ret) );
		__manufacturerName = _cBuf;
	} else	// no iManufacturer specified
		__manufacturerName = "Unknown";

	if ( _desc.iProduct != 0 ){
		_ret = libusb_get_string_descriptor_ascii( __handle, _desc.iProduct, (unsigned char *)_cBuf, sizeof(_cBuf) );
		if (_ret < 0)
			throw( rUSB_exception("rUSB_interface constructor : error on retrieving string descriptor of product ID.", _ret) );
		__productName = _cBuf;
	} else	// no iProduct specified
		__productName = "Unknown";

	// Get serial number, if any
	if ( _desc.iSerialNumber != 0 ){
		_ret = libusb_get_string_descriptor_ascii( __handle, _desc.iSerialNumber, (unsigned char *)_cBuf, sizeof(_cBuf) );
		if (_ret < 0)
			throw( rUSB_exception("rUSB_interface constructor : error on retrieving string descriptor of serial number.", _ret) );
		__serialNo = atoi(_cBuf);
	} else	// no iSerialNumber specified
		__serialNo = 0;

	gatherInterfaceInfo();

	// Done! Device initialized properly, increase device count
	__device_count++; 		
}

void rUSB_interface::gatherInterfaceInfo(){

	/*
	 *	Gathering configuration information (endpoint addresses, IN/OUT config, endpoint types..)
	 */

	libusb_config_descriptor *_config_desc;
	const libusb_interface *_interface;
	const libusb_interface_descriptor *_interface_desc;
	const libusb_endpoint_descriptor *_ep_desc;

	int _nSettings, _nEndpoints;
	struct rUSB_endpoint _ep_info;

	// Look at config 0 (there are very few defices with alternate configs TODO!)
	libusb_get_config_descriptor(__device, 0, &_config_desc);
	// Save number of interfaces on the device
	__nInterfaces = (int)_config_desc->bNumInterfaces;

	// Loop over all interfaces on this config
	for(int iInterface = 0; iInterface < __nInterfaces; iInterface++) {
		_interface = &_config_desc->interface[iInterface];	// Get an interface
		// Store number of settings for this interface
	    	_nSettings = _interface->num_altsetting;	
		_ep_info.interfaceNo = iInterface;

		// Loop over all settings of the interface
	    	for(int iSetting = 0; iSetting < _nSettings; iSetting++) {
			_interface_desc = &_interface->altsetting[iSetting];	// Get alt setting
			// Store number of endpoints for this setting
			_nEndpoints = (int)_interface_desc->bNumEndpoints;

			_ep_info.settingNo = iSetting;

			for(int k = 0; k < _nEndpoints; k++) {
		    		_ep_desc = &_interface_desc->endpoint[k];	// Get endpoint

				// Fill EP info struct
				_ep_info.address = 0x7F & (int)_ep_desc->bEndpointAddress;	// Mask out highest bit
				_ep_info.type = (int)_ep_desc->bmAttributes;

				if (_ep_desc->bEndpointAddress < 0x80){
					_ep_info.isOUT = true;
					_ep_info.isIN = false;
				} else {
					_ep_info.isOUT = false;
					_ep_info.isIN = true;
				}

				// Save struct	
				__endpoints.push_back(_ep_info);	
			}
	    	}
	}

	__nEndpoints = __endpoints.size();
	// Free config descriptor after usage
	libusb_free_config_descriptor (_config_desc);

	// Loop over all endpoint information gathered and extract default serial communication parameters:
	//	- the first interface with an IN or OUT endpoint will become the default interface
	//	- if there are no interfaces implemented on the device the communication will go to EP0

	__defaultEndpointIn = 0x00; 
	__defaultEndpointOut = 0x00; 
	__defaultInterface = RUSB_NO_INTERFACE;
	__defaultEndpointInType = LIBUSB_TRANSFER_TYPE_CONTROL;
	__defaultEndpointOutType = LIBUSB_TRANSFER_TYPE_CONTROL;

	for (int _iEndpoint = 0; _iEndpoint < __nEndpoints; _iEndpoint++){
		// Take first interface as default interface 
		if (__defaultInterface == RUSB_NO_INTERFACE){	
			__defaultInterface = __endpoints.at(_iEndpoint).interfaceNo;
		#ifdef DEBUG
			printf("[DEBUG rUSB_interface] - Chosen default interface %i.\n", __defaultInterface);
		#endif
		}

		// Stop looping as soon as interface changes
		if (__defaultInterface != __endpoints.at(_iEndpoint).interfaceNo) break;

		// Write back endpoints
		if (__endpoints.at(_iEndpoint).isOUT){
			__defaultEndpointOut = __endpoints.at(_iEndpoint).address + 0x80;	
			__defaultEndpointOutType = __endpoints.at(_iEndpoint).type;
		#ifdef DEBUG
			printf("[DEBUG rUSB_interface] - Default out endpoint 0x%02X, type %i.\n", __defaultEndpointOut, __defaultEndpointOutType);
		#endif
		} else {
			__defaultEndpointIn = __endpoints.at(_iEndpoint).address;	
			__defaultEndpointInType = __endpoints.at(_iEndpoint).type;
		#ifdef DEBUG
			printf("[DEBUG rUSB_interface] - Default in endpoint 0x%02X, type %i.\n", __defaultEndpointIn, __defaultEndpointInType);
		#endif
		}
	}

#ifdef DEBUG
	if (__defaultInterface == RUSB_NO_INTERFACE)
		printf("[DEBUG rUSB_interface] - Default communication to EP0.\n");
#endif

	return;
}

#endif
