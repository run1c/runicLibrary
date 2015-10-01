#ifndef R_USB_INTERFACE_H
#define R_USB_INTERFACE_H

/*
 *	Exception class for error handling
 */

#include <rException.h>

class rUSB_exception : public rException{

public:
	rUSB_exception(std::string _message, int _errNumber=0) : rException("[rUSB_exception] - " + _message, _errNumber) {};
	~rUSB_exception() throw() {};

};

/*
 *	Software interface for communication with USB devices using libusb.
 */

#include <rSerial_interface.h>
#include <libusb-1.0/libusb.h>
#include <vector>

#define RUSB_BUF_LEN			64	
#define RUSB_DEFAULT_TIMEOUT_MS		1
#define RUSB_MAX_TRIES			10	

#define RUSB_NO_INTERFACE		-1	// No interface currently selected; Iso- & bulktransfers will fail, controltransfers will go to EP0

/*
 *	Struct for endpoint information
 */

struct rUSB_endpoint{
	uint8_t address;
	uint8_t type;
	bool isOUT;
	bool isIN;
	int interfaceNo;
	int settingNo;
};

class rUSB_interface : public rSerial_interface {

public:
	// Default constructor for arrays
	rUSB_interface(); 

	// Open USB device with given vendor and product ID
	rUSB_interface(uint16_t _vid, uint16_t _pid) throw(rUSB_exception);

	// Open USB device with handle 	
	rUSB_interface(libusb_device_handle *_handle) throw(rUSB_exception);

	// Default destructor
	~rUSB_interface();

	// Default send method
	int sendOut(const uint8_t *_data, int _len) throw(rUSB_exception);
	
	// Default read method
	int readIn(uint8_t *_data, int _maxLen, int _timeout_ms = RUSB_DEFAULT_TIMEOUT_MS) throw(rUSB_exception);

	// Claim interface to communicate with the endpoints of the interface; claim RUSB_NO_INTERFACE to communicate with EP0
	void claimInterface(int _interfaceNo = RUSB_NO_INTERFACE) throw(rUSB_exception);

	// Control transfer
	int controlTransfer(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, uint8_t _endpoint = 0, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);
	// Send a control package to receive data from the device, _buffer contains the payload defined by _request
	int readInControl(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);	
	int readInControl(uint8_t _request, unsigned char* _buffer, int _len, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);
	// Sends a control package with data for the device 
	int sendOutControl(uint8_t _request, uint16_t _index, uint16_t _value, unsigned char* _buffer, int _len, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);	
	int sendOutControl(uint8_t _request, unsigned char* _buffer, int _len, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);

	// Bulk transfer
	int bulkTransfer(unsigned char* _data, int _len, uint8_t _endpoint, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);
	// Send data using bulk transfer
	int sendOutBulk(unsigned char* _data, int _len, uint8_t _endpoint = 1, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);
	// Read in data using bulk transfer
	int readInBulk(unsigned char* _data, int _len, uint8_t _endpoint = 1, int _maxTries = RUSB_MAX_TRIES) throw(rUSB_exception);

	// Simple getter methods
	std::string getManufacturerName(); 
	std::string getProductName(); 
	int getSerialNumber();
	int getBusNumber();
	int getDevAddress();
	uint16_t getVendorID();
	uint16_t getProductID();
	int getNInterfaces();
	int getNEndpoints();
	// Get struct containing information about the endpoint
	struct rUSB_endpoint getEndpointInfo(int _iEndpoint);

	// Print info about the device
	void printInfo();
	
	// Static methods for libusb_context
	static libusb_context* getContext() throw(rUSB_exception);
	static void setContext(libusb_context *_context) throw(rUSB_exception);
	static int getDevCount();

private:
	// Initialization used in constructors
	void init(libusb_device_handle *_handle) throw(rUSB_exception);

	// Gather info about the functions (interfaces) of the device
	void gatherInterfaceInfo();

	// Context for ALL usb devices using an rUSB_interface
	static libusb_context* __context;
	static int __device_count;

	uint16_t __vid, __pid;
	std::string __manufacturerName, __productName; 
	int __serialNo, __busNumber, __devAddress;

	// Interface & endpoint infos
	int __curInterfaceNo, __nInterfaces, __nEndpoints;	// Current interface & number of interfaces and endpoints on device
	std::vector<struct rUSB_endpoint> __endpoints;

	// Default settings used by sendOut/readIn methods
	int __defaultInterface, __defaultEndpointInType, __defaultEndpointOutType;
	uint8_t __defaultEndpointIn, __defaultEndpointOut;

	// Libusb objects
	libusb_device_handle *__handle;
	libusb_device *__device;
};

#endif
