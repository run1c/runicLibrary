#ifndef HANTEK_DSO_5000P_CPP
#define HANTEK_DSO_5000P_CPP

#include "HantekDSO_5000P.h"
#include <unistd.h>
#include <stdio.h>

//#define DEBUG

HantekDSO_5000P::HantekDSO_5000P(rUSB_interface *_port) throw(HantekDSO_exception){
	__usb_port = _port;

	// Check vendor id (Hantek)
	if ( __usb_port->getVendorID() != HANTEK_DSO_5072P_VID )
		throw ( HantekDSO_exception("constructor : wrong VID.") );

	// Check product id (might add some other pid later...)
	switch ( __usb_port->getProductID() ){
	case HANTEK_DSO_5072P_PID:
		break;
	default:
		throw ( HantekDSO_exception("constructor : PID does not match any supported devices.") );
	}

	// Check endpoints in all interfaces
	struct rUSB_endpoint _ep_in, _ep_out;
	for (int _iInterface = 0; _iInterface < __usb_port->getNInterfaces(); _iInterface++ ){
		for (int _iEP = 0; _iEP < __usb_port->getNEndpoints(); _iEP++ ){
			// Check for endpoint addresses
			if ( __usb_port->getEndpointInfo(_iEP).address == HANTEK_DSO_EP_OUT){
				_ep_out = __usb_port->getEndpointInfo(_iEP);
				continue;
			}
			if ( __usb_port->getEndpointInfo(_iEP).address == HANTEK_DSO_EP_IN){
				_ep_in = __usb_port->getEndpointInfo(_iEP);
				continue;
			}
		}	
	}
	if ( (_ep_in.interfaceNo == _ep_out.interfaceNo) &&	// Both endpoints have to belong to the same interface
	      _ep_in.isIN && _ep_out.isOUT ){			// Check for IN & OUT
		// Save all EP settings
		__ioInterfaceNo = _ep_in.interfaceNo;
		__ep_in_addr = _ep_in.address;
		__ep_out_addr = _ep_out.address;
	} else
		throw( HantekDSO_exception("constructor : wrong endpoint configuration found.") );
	// Claim the io interface
	__usb_port->claimInterface(__ioInterfaceNo);	 

	// Flush RX buffer
	this->flushBuffer();
}

HantekDSO_5000P::~HantekDSO_5000P(){
	// Free interfaces
	__usb_port->claimInterface(RUSB_NO_INTERFACE);	 
}

void HantekDSO_5000P::beep() throw(rUSB_exception, HantekDSO_exception) {
	uint8_t _data_buffer[10];
	_data_buffer[0] = 0x02;	// 100ms/count
	issueRequest(HANTEK_DSO_DEBUG_MSG, HANTEK_DSO_BEEP, _data_buffer, 1);
	return;
};

void HantekDSO_5000P::lockPanel() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[10];
	_data_buffer[0]= 0x01;	// Lock/unlock subcommand
	_data_buffer[1]= 0x01;	// Lock
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_START_STOP, _data_buffer, 2);
	return;
};

void HantekDSO_5000P::unlockPanel() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[10];
	_data_buffer[0]= 0x01;	// Lock/unlock subcommand
	_data_buffer[1]= 0x00;	// Unlock
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_START_STOP, _data_buffer, 2);
	return;
};

void HantekDSO_5000P::startDAQ() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[10];
	_data_buffer[0]= 0x00;	// DAQ subcommand
	_data_buffer[1]= 0x00;	// Start
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_START_STOP, _data_buffer, 2);
	return;
};

void HantekDSO_5000P::stopDAQ() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[10];
	_data_buffer[0]= 0x00;	// DAQ subcommand
	_data_buffer[1]= 0x01;	// Stop
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_START_STOP, _data_buffer, 2);
	return;
};

void HantekDSO_5000P::keyPress(uint8_t _key) throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[10];
	_data_buffer[0]= _key;	
	_data_buffer[1]= 0x01;
	issueRequest(HANTEK_DSO_NORMAL_MSG, 0x13, _data_buffer, 2);
	return;
};

void HantekDSO_5000P::getSettings() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _buf[300];

	issueRequest(HANTEK_DSO_NORMAL_MSG, 0x01, NULL, 0, _buf, sizeof(_buf) );
	// Remove 4 header bytes and 1 footer byte
	for (int i = 0; i < sizeof(__DSOsettings.raw); i++)
		__DSOsettings.raw[i] = _buf[i+4];

	// Extract settings!
	__voltsPerDiv[0] = getVoltsPerDiv(__DSOsettings.sysData.CH1_volt_per_div);
	__voltsPerDiv[1] = getVoltsPerDiv(__DSOsettings.sysData.CH2_volt_per_div);
	__probe_attenuation[0] = getAttenuation(__DSOsettings.sysData.CH1_probe);
	__probe_attenuation[1] = getAttenuation(__DSOsettings.sysData.CH2_probe);
	__channelEnable[0] = __DSOsettings.sysData.CH1_on;
	__channelEnable[1] = __DSOsettings.sysData.CH2_on;
	__channelOffset[0] = (int16_t)__DSOsettings.sysData.CH1_position;
	__channelOffset[1] = (int16_t)__DSOsettings.sysData.CH2_position;
	__secondsPerDiv = getSecPerDiv(__DSOsettings.sysData.timebase);
	return;
}

HantekDSO_settings HantekDSO_5000P::Settings() {
	return __DSOsettings;
}

void HantekDSO_5000P::applySettings() throw(rUSB_exception, HantekDSO_exception) {
	uint8_t _buf[30];
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_APPL_SETTING, __DSOsettings.raw, sizeof(__DSOsettings), _buf, 30);
	return;	
}

void HantekDSO_5000P::applySettings(HantekDSO_settings _settings) throw(rUSB_exception, HantekDSO_exception) {
	uint8_t _buf[30];
	issueRequest(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_APPL_SETTING, _settings.raw, sizeof(_settings), _buf, 30);
	return;	
}

uint8_t HantekDSO_5000P::getTriggerState() throw(rUSB_exception, HantekDSO_exception){
	uint8_t _buf[300];

	issueRequest(HANTEK_DSO_NORMAL_MSG, 0x01, NULL, 0, _buf, sizeof(_buf) );
	return _buf[24];
}

void HantekDSO_5000P::enableCH(int _chNo, bool _on_off) {
	switch (_chNo) {
		case 1:
		__DSOsettings.sysData.CH1_on = (uint8_t)_on_off;
		break;
		case 2:
		__DSOsettings.sysData.CH2_on = (uint8_t)_on_off;
		break;
		default:	
		// TODO: Exception, wrong channel value!!
		break;	
	}
	return;
}

void HantekDSO_5000P::setCoupling(int _chNo, uint8_t _vert_coup) {
	// TODO: check _vert_coup for valid value
	switch (_chNo) {
		case 1:
		__DSOsettings.sysData.CH1_coupling = _vert_coup; 
		break;
		case 2:
		__DSOsettings.sysData.CH2_coupling = _vert_coup; 
		break;
		default:	
		// TODO: Exception, wrong channel value!!
		break;	
	}
	return;
}

void HantekDSO_5000P::setVertical(int _chNo, uint8_t _vert_VB) {
	// TODO: check _vert_VB for valid value
	switch (_chNo) {
		case 1:
		__DSOsettings.sysData.CH1_volt_per_div = _vert_VB; 
		break;
		case 2:
		__DSOsettings.sysData.CH2_volt_per_div = _vert_VB; 
		break;
		default:	
		// TODO: Exception, wrong channel value!!
		break;	
	}
	return;
}

void HantekDSO_5000P::setAttenuation(int _chNo, uint8_t _vert_probe) {
	// TODO: check _vert_probe for valid value
	switch (_chNo) {
		case 1:
		__DSOsettings.sysData.CH1_probe = _vert_probe;
		break;
		case 2:
		__DSOsettings.sysData.CH2_probe = _vert_probe;
		break;
		default:	
		// TODO: Exception, wrong channel value!!
		break;	
	}
	return;
}

void HantekDSO_5000P::setVertPos(int _chNo, int _pos) {
	// TODO: check _pos for valid value
	// Auf den Screen passen etwa +-100 Werte (4 division = 100 Werte)
	switch (_chNo) {
		case 1:
		__DSOsettings.sysData.CH1_position = _pos;
		break;
		case 2:
		__DSOsettings.sysData.CH2_position = _pos;
		break;
		default:	
		// TODO: Exception, wrong channel value!!
		break;	
	}
	return;
}

void HantekDSO_5000P::setTriggerMode(uint8_t _mode) {
	// TODO: check _timebase for valid value
	__DSOsettings.raw[118] = _mode;		// Through trial and error...
//	__DSOsettings.sysData.trig_mode = _mode;
	return;
}

void HantekDSO_5000P::setTriggerLevel(int _trig_pos) {
	// TODO: check _trig_pos for valid value
	// Attention: Lower byte first, upper second 
	uint8_t _trig_posu = (uint8_t) (_trig_pos >> 8);
	uint8_t _trig_posl = (uint8_t) (_trig_pos & 0xFF);
//	__DSOsettings.sysData.trig_vpos = (uint16_t) ( (_trig_posl << 8) | _trig_posu); // Does not work... better with RAW and defined positions
	__DSOsettings.raw[25] = _trig_posl;
	__DSOsettings.raw[26] = _trig_posu;
	return;
}

void HantekDSO_5000P::setTimebase(uint8_t _timebase) {
	// TODO: check _timebase for valid value
	__DSOsettings.sysData.timebase = _timebase;
	__DSOsettings.sysData.window_timebase = _timebase;
	return;
}

void HantekDSO_5000P::setTriggerDelay(long long int _delay) {
	// TODO: check _delay for valid value
	// Each step is 50ps
	__DSOsettings.raw[163] = (_delay & 0x00000000000000FF) >> 0;
	__DSOsettings.raw[164] = (_delay & 0x000000000000FF00) >> 8;
	__DSOsettings.raw[165] = (_delay & 0x0000000000FF0000) >> 16;
	__DSOsettings.raw[166] = (_delay & 0x00000000FF000000) >> 24;
	__DSOsettings.raw[167] = (_delay & 0x000000FF00000000) >> 32;
	__DSOsettings.raw[168] = (_delay & 0x0000FF0000000000) >> 40;
	__DSOsettings.raw[169] = (_delay & 0x00FF000000000000) >> 48;
	__DSOsettings.raw[170] = (_delay & 0xFF00000000000000) >> 56;
	return;
}

int HantekDSO_5000P::readSampleData(uint8_t _channel, double *_data, double *_time, int _len, int _delay_us) throw(rUSB_exception, HantekDSO_exception){
	uint8_t _data_buffer[11000];
	int _sample_len = HANTEK_DSO_NODATA, _packet_len, _sampleByteReceived = 0;

	if (__channelEnable[_channel] == false)
		throw( rUSB_exception("readSampleData : channel disabled.") );
	// Prepare request	
	_data_buffer[0] = 0x01;	// Subcommand
	_data_buffer[1] = _channel; 

	// Send request for data
	sendOut(HANTEK_DSO_NORMAL_MSG, HANTEK_DSO_READ_SAMPLE, _data_buffer, 2);

	while (true) {
		// Optional sleep for larger data packages
		usleep(_delay_us);

		// Read packet
		_packet_len = readIn( _data_buffer, sizeof(_data_buffer) );
		
		// Check for error packets
		if (_data_buffer[HANTEK_DSO_SUB_CMD_POS] == HANTEK_DSO_SUB_ERROR)
			break; // throw( HantekDSO_exception("readSampleData : Received error package.") );

		// Get sample length from first data packet
		if (_data_buffer[HANTEK_DSO_SUB_CMD_POS] == HANTEK_DSO_SUB_FIRST){
			_sample_len = _data_buffer[5] + (_data_buffer[6] << 8) + (_data_buffer[7] << 16);	
			if (_len < _sample_len)
				throw( HantekDSO_exception("readSampleData : data buffer too small.") );
		}

		// Get sample data from continous packets
		if (_data_buffer[HANTEK_DSO_SUB_CMD_POS] == HANTEK_DSO_SUB_CONT){
			// Sample data begins two bytes after the subcommand byte (CHn) and ends one byte before end of the sample (checksum) 
			for (int _iByte = (HANTEK_DSO_SUB_CMD_POS + 2); _iByte < (_packet_len - 1); _iByte++){
				// Calculate actual voltage from read settings
				_data[_sampleByteReceived] = ( (int8_t)_data_buffer[_iByte] - __channelOffset[_channel] ) * __probe_attenuation[_channel] * __voltsPerDiv[_channel]/HANTEK_DSO_VERT_DOTS_PER_DIV;
				_time[_sampleByteReceived] = _sampleByteReceived * __secondsPerDiv * HANTEK_DSO_HORZ_DIV_PER_SCREEN/_sample_len; 
				_sampleByteReceived++;
			}
		}

		// Stop looping if last packet has been received
		if (_data_buffer[HANTEK_DSO_SUB_CMD_POS] == HANTEK_DSO_SUB_LAST) break;
	}
#ifdef DEBUG
	printf("[DEBUG HantekDSO_5000P::readSampleData] - Sample length %i.\n", _sample_len);
#endif
	return _sample_len;	
}

/*	
 *	P R I V A T E   M E T H O D S
 */

int HantekDSO_5000P::sendOut(uint8_t _marker, uint8_t _cmd, uint8_t *_data, int _len) throw(rUSB_exception){
	int _iByte, _checksum = 0;
	int _total_len = _len + 5;	// Five additional bytes: _marker, 2 length bytes, 1 command, 1 checksum
	uint8_t *_data_buffer = new uint8_t[_total_len];	

	// Prepare data package according to protocol
	_data_buffer[HANTEK_DSO_MARKER_POS] = _marker;			// Marker byte
	_data_buffer[HANTEK_DSO_LLEN_POS] = 0xFF & (_len + 2);		// Lower length byte
	_data_buffer[HANTEK_DSO_HLEN_POS] = 0xFF & ((_len + 2) >> 8);	// Higer length byte
	_data_buffer[HANTEK_DSO_CMD_POS] = _cmd;			// Command byte
	// Assign data bytes
	for (_iByte = 0; _iByte < _len; _iByte++) 
		_data_buffer[_iByte + HANTEK_DSO_DATA_FIRST] = _data[_iByte];

	// Calculate checksum
	for (int i = 0; i < (_total_len - 1); i++)
		_checksum += _data_buffer[i];
	_data_buffer[_total_len - 1] = 0xFF & _checksum;	

#ifdef DEBUG
	printf("[DEBUG HantekDSO_5000P::sendOut] - Sending: ");
	for (int i = 0; i < _total_len; i++)
		printf("0x%02X[%i] ", _data_buffer[i], i);
	printf(" - Sent %i bytes\n", _total_len);	
#endif

	// Send data via USB and return number of transferred bytes
	usleep(HANTEK_DSO_IDLE_US);	// Spacer between successive reads/writes
	return __usb_port->sendOutBulk( _data_buffer, _total_len, __ep_out_addr , HANTEK_DSO_MAXTRIES );	
}

int HantekDSO_5000P::readIn(uint8_t *_data, int _max_len) throw(rUSB_exception, HantekDSO_exception) {
	int _bytesTransferred = -1, _checksum = 0;

	// Read data via USB
	_bytesTransferred = __usb_port->readInBulk( _data, _max_len, __ep_in_addr, HANTEK_DSO_MAXTRIES );

#ifdef DEBUG
	printf("[DEBUG HantekDSO_5000P::readIn] - Received: ");
	for (int i = 0;i < (_bytesTransferred-1); i++){ 
		printf("0x%02X[%i] ", _data[i], i);
	}
	printf("0x%02X - Received %i bytes\n", _data[_bytesTransferred-1], _bytesTransferred);
#endif

	// Check command byte direction (CMD > 0x80 = DSO -> PC)
	if ( _data[HANTEK_DSO_CMD_POS] < HANTEK_DSO_IN_CMD )
		throw( HantekDSO_exception("readIn : no command byte received.") );	

	// Calculate checksum
	for (int _iByte = 0; _iByte < (_bytesTransferred - 1); _iByte++){
		_checksum += _data[_iByte];
	}
	_checksum &= 0xFF;

	// Compare checksum	
	if (_data[_bytesTransferred - 1] != _checksum)
		throw ( HantekDSO_exception("readIn : wrong checksum.") );

	return _bytesTransferred;
}
	
void HantekDSO_5000P::flushBuffer(){
#ifdef DEBUG
	printf("[DEBUG HantekDSO_5000P::flushBuffer] - Reading in buffer.\n");
#endif
	uint8_t _dummy[13000];
	while (true) {
		try{
			if ( readIn(_dummy, sizeof(_dummy) ) == -1 ) break;
		} catch (rUSB_exception &ex) {
#ifdef DEBUG
			printf("[DEBUG HantekDSO_5000P::flushBuffer] - Timeout.\n");
#endif
			break;
		} catch (HantekDSO_exception) {
			break;
		}
	}
#ifdef DEBUG
	printf("[DEBUG HantekDSO_5000P::flushBuffer] - Buffer empty!\n");
#endif
	return;
}

void HantekDSO_5000P::issueRequest(uint8_t _marker, uint8_t _cmd, uint8_t *_dataOut, int _lenOut, uint8_t *_dataIn, int _lenIn) throw(rUSB_exception, HantekDSO_exception){
#ifdef DEBUG
	printf("\n[DEBUG HantekDSO_5000P::issueRequest] ################# \n");
#endif
	// Send request
	sendOut(_marker, _cmd, _dataOut, _lenOut);

	// Read answer
	while (true) {
		// Read until a reasonable answer was received
		readIn( _dataIn, _lenIn );

		if ( _dataIn[HANTEK_DSO_CMD_POS] == (HANTEK_DSO_IN_CMD + _cmd) ) break;
#ifdef DEBUG
		printf("[DEBUG HantekDSO_5000P::issueRequest] - Wrong message, reading again.\n");
#endif
	}	
	
	return;
}

void HantekDSO_5000P::issueRequest(uint8_t _marker, uint8_t _cmd, uint8_t *_data, int _len) throw(rUSB_exception, HantekDSO_exception){
	uint8_t _buf[300];
	issueRequest(_marker, _cmd, _data, _len, _buf, sizeof(_buf));
	return;
}

float HantekDSO_5000P::getVoltsPerDiv(uint8_t _vb){
	switch (_vb) {
	case HANTEK_DSO_VERT_VB_2MV:
		return 0.002;
		break;
	case HANTEK_DSO_VERT_VB_5MV:
		return 0.005;
		break;
	case HANTEK_DSO_VERT_VB_10MV:
		return 0.010;
		break;
	case HANTEK_DSO_VERT_VB_20MV:
		return 0.020;
		break;
	case HANTEK_DSO_VERT_VB_50MV:
		return 0.050;
		break;
	case HANTEK_DSO_VERT_VB_100MV:
		return 0.100;
		break;
	case HANTEK_DSO_VERT_VB_200MV:
		return 0.200;
		break;
	case HANTEK_DSO_VERT_VB_500MV:
		return 0.500;
		break;
	case HANTEK_DSO_VERT_VB_1V:
		return 1.000;
		break;
	case HANTEK_DSO_VERT_VB_2V:
		return 2.000;
		break;
	case HANTEK_DSO_VERT_VB_5V:
		return 5.000;
		break;
	default:
		break;
	}
	return -1;
}

double HantekDSO_5000P::getAttenuation(uint8_t _att){
	switch (_att) {
	case HANTEK_DSO_VERT_PROBE_1:
		return 1.;
		break;
	case HANTEK_DSO_VERT_PROBE_10:
		return 10.;
		break;
	case HANTEK_DSO_VERT_PROBE_100:
		return 100.;
		break;
	case HANTEK_DSO_VERT_PROBE_1000:
		return 1000.;
		break;
	default:
		break;
	}
	return -1;
}

double HantekDSO_5000P::getSecPerDiv(uint8_t _tb){
	switch (_tb) {
	case HANTEK_DSO_HORZ_TB_2NS:
		return 2e-9;
		break;
	case HANTEK_DSO_HORZ_TB_4NS:	
		return 4e-9;
		break;
        case HANTEK_DSO_HORZ_TB_8NS:	
		return 8e-9;
		break;
        case HANTEK_DSO_HORZ_TB_20NS:	
		return 20e-9;
		break;
        case HANTEK_DSO_HORZ_TB_40NS:	
		return 40e-9;
		break;
        case HANTEK_DSO_HORZ_TB_80NS:	
		return 80e-9;
		break;
        case HANTEK_DSO_HORZ_TB_200NS:
		return 200e-9;
		break;
        case HANTEK_DSO_HORZ_TB_400NS:
		return 400e-9;
		break;
        case HANTEK_DSO_HORZ_TB_800NS:
		return 800e-9;
		break;
        case HANTEK_DSO_HORZ_TB_2US:	
		return 2e-6;
		break;
        case HANTEK_DSO_HORZ_TB_4US:	
		return 4e-6;
		break;
        case HANTEK_DSO_HORZ_TB_8US:	
		return 8e-6;
		break;
        case HANTEK_DSO_HORZ_TB_20US:	
		return 20e-6;
		break;
        case HANTEK_DSO_HORZ_TB_40US:	
		return 40e-6;
		break;
        case HANTEK_DSO_HORZ_TB_80US:	
		return 80e-6;
		break;
        case HANTEK_DSO_HORZ_TB_200US:
		return 200e-6;
		break;
        case HANTEK_DSO_HORZ_TB_400US:
		return 400e-6;
		break;
        case HANTEK_DSO_HORZ_TB_800US:
		return 800e-6;
		break;
        case HANTEK_DSO_HORZ_TB_2MS:	
		return 2e-3;
		break;
        case HANTEK_DSO_HORZ_TB_4MS:	
		return 4e-3;
		break;
        case HANTEK_DSO_HORZ_TB_8MS:	
		return 8e-3;
		break;
        case HANTEK_DSO_HORZ_TB_20MS:	
		return 20e-3;
		break;
        case HANTEK_DSO_HORZ_TB_40MS:	
		return 40e-3;
		break;
        case HANTEK_DSO_HORZ_TB_80MS:	
		return 80e-3;
		break;
        case HANTEK_DSO_HORZ_TB_200MS:
		return 200e-3;
		break;
        case HANTEK_DSO_HORZ_TB_400MS:
		return 400e-3;
		break;
	case HANTEK_DSO_HORZ_TB_800MS:
		return 800e-3;
		break;
	case HANTEK_DSO_HORZ_TB_2S:
		return 2.;
		break;
	case HANTEK_DSO_HORZ_TB_4S:	
		return 4.;
		break;
        case HANTEK_DSO_HORZ_TB_8S:	
		return 8.;
		break;
	case HANTEK_DSO_HORZ_TB_20S:	
		return 20.;
		break;
 	case HANTEK_DSO_HORZ_TB_40S:	
		return 40.;
		break;
        case HANTEK_DSO_HORZ_TB_80S:	
		return 80.;
		break;
	default:
		break;
	}
	return -1;
}

#endif
