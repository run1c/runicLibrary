#ifndef HANTEK_DSO_5000P_H
#define HANTEK_DSO_5000P_H

/*
 *      Exception class for error handling
 */

#include <rException.h>

class HantekDSO_exception : public rException{

public:
        HantekDSO_exception(std::string _message, int _errNumber=0) : rException("[HantekDSO_exception] - " + _message, _errNumber) {};
        ~HantekDSO_exception() throw() {};

};
/*
 *	Class for controlling/monitoring Hantek DSO 5000P series
 */

#include <usb/rUSB_interface.h>

#define	HANTEK_DSO_5072P_VID	0x049F
#define	HANTEK_DSO_5072P_PID	0x505A

#define HANTEK_DSO_EP_OUT	0x02
#define HANTEK_DSO_EP_IN	0x01

// Request format
#define HANTEK_DSO_MARKER_POS	0
#define HANTEK_DSO_LLEN_POS	1
#define HANTEK_DSO_HLEN_POS	2
#define HANTEK_DSO_CMD_POS	3
#define HANTEK_DSO_DATA_FIRST	4
#define HANTEK_DSO_SUB_CMD_POS	4
#define HANTEK_DSO_IN_CMD	0x80

#define HANTEK_DSO_USB_MAXTRIES	2
#define HANTEK_DSO_IDLE_US	2e4	// Delay between read and write commands; needed for stable readout
#define HANTEK_DSO_MAXTRIES	2

// Message types
#define HANTEK_DSO_NORMAL_MSG	0x53
#define HANTEK_DSO_DEBUG_MSG	0x43

// Commands
#define HANTEK_DSO_READ_SETTING	0x01	
#define HANTEK_DSO_APPL_SETTING	0x11	
#define HANTEK_DSO_SCREENSHOT	0x20	
#define HANTEK_DSO_READ_TIME	0x21	
#define HANTEK_DSO_BEEP		0x44
#define HANTEK_DSO_INIT		0x7F

// Stop/start data aquisition, lock/unlock panel
#define HANTEK_DSO_START_STOP	0x12
#define HANTEK_DSO_SUB_DAQ	0x00
#define HANTEK_DSO_SUB_LOCK	0x01

// Read sample including subcommands
#define HANTEK_DSO_READ_SAMPLE	0x02
#define HANTEK_DSO_N_CHANNELS	2
#define HANTEK_DSO_CH1		0x00
#define HANTEK_DSO_CH2		0x01
#define HANTEK_DSO_SUB_FIRST	0x00
#define HANTEK_DSO_SUB_CONT	0x01
#define HANTEK_DSO_SUB_LAST	0x02
#define HANTEK_DSO_SUB_ERROR	0x03
#define HANTEK_DSO_MAX_PACKETS	202
#define HANTEK_DSO_NODATA	-1	

// Settings length (208 bytes of settings)
#define HANTEK_DSO_SETTINGS_LEN		208
// Vertical settings (Volts/division) 
#define HANTEK_DSO_VERT_DIV_PER_SCREEN	8.32f
#define HANTEK_DSO_VERT_DOTS_PER_DIV	25.
#define HANTEK_DSO_VERT_VB_2MV		0x00
#define HANTEK_DSO_VERT_VB_5MV		0x01
#define HANTEK_DSO_VERT_VB_10MV		0x02
#define HANTEK_DSO_VERT_VB_20MV		0x03
#define HANTEK_DSO_VERT_VB_50MV		0x04
#define HANTEK_DSO_VERT_VB_100MV	0x05
#define HANTEK_DSO_VERT_VB_200MV	0x06
#define HANTEK_DSO_VERT_VB_500MV	0x07
#define HANTEK_DSO_VERT_VB_1V		0x08
#define HANTEK_DSO_VERT_VB_2V		0x09
#define HANTEK_DSO_VERT_VB_5V		0x0A
// Coupling settings
#define HANTEK_DSO_COUP_DC		0x00
#define HANTEK_DSO_COUP_AC		0x01
#define HANTEK_DSO_COUP_GND		0x02
// Probe attenuation
#define HANTEK_DSO_VERT_PROBE_1		0x00
#define HANTEK_DSO_VERT_PROBE_10	0x01
#define HANTEK_DSO_VERT_PROBE_100	0x02
#define HANTEK_DSO_VERT_PROBE_1000	0x03

// Horizontal settings (Seconds/division)
#define HANTEK_DSO_HORZ_DIV_PER_SCREEN	16.f
#define HANTEK_DSO_HORZ_TB_2NS		0x00
#define HANTEK_DSO_HORZ_TB_4NS		0x01
#define HANTEK_DSO_HORZ_TB_8NS		0x02
#define HANTEK_DSO_HORZ_TB_20NS		0x03
#define HANTEK_DSO_HORZ_TB_40NS		0x04
#define HANTEK_DSO_HORZ_TB_80NS		0x05
#define HANTEK_DSO_HORZ_TB_200NS	0x06
#define HANTEK_DSO_HORZ_TB_400NS	0x07
#define HANTEK_DSO_HORZ_TB_800NS	0x08
#define HANTEK_DSO_HORZ_TB_2US		0x09
#define HANTEK_DSO_HORZ_TB_4US		0x0A
#define HANTEK_DSO_HORZ_TB_8US		0x0B
#define HANTEK_DSO_HORZ_TB_20US		0x0C
#define HANTEK_DSO_HORZ_TB_40US		0x0D
#define HANTEK_DSO_HORZ_TB_80US		0x0E
#define HANTEK_DSO_HORZ_TB_200US	0x0F
#define HANTEK_DSO_HORZ_TB_400US	0x10
#define HANTEK_DSO_HORZ_TB_800US	0x11
#define HANTEK_DSO_HORZ_TB_2MS		0x12
#define HANTEK_DSO_HORZ_TB_4MS		0x13
#define HANTEK_DSO_HORZ_TB_8MS		0x14
#define HANTEK_DSO_HORZ_TB_20MS		0x15
#define HANTEK_DSO_HORZ_TB_40MS		0x16
#define HANTEK_DSO_HORZ_TB_80MS		0x17
#define HANTEK_DSO_HORZ_TB_200MS	0x18
#define HANTEK_DSO_HORZ_TB_400MS	0x19
#define HANTEK_DSO_HORZ_TB_800MS	0x1A
#define HANTEK_DSO_HORZ_TB_2S		0x1B
#define HANTEK_DSO_HORZ_TB_4S		0x1C
#define HANTEK_DSO_HORZ_TB_8S		0x1D
#define HANTEK_DSO_HORZ_TB_20S		0x1E
#define HANTEK_DSO_HORZ_TB_40S		0x1F
#define HANTEK_DSO_HORZ_TB_80S		0x20

// Trigger status
#define HANTEK_DSO_TRIG_STOP		0x00
#define HANTEK_DSO_TRIG_READY		0x01
#define HANTEK_DSO_TRIG_AUTO		0x02
#define HANTEK_DSO_TRIG_TRIGGERED	0x03
#define HANTEK_DSO_TRIG_SCAN		0x04
#define HANTEK_DSO_TRIG_ASTOP		0x05
#define HANTEK_DSO_TRIG_ARMED		0x06
// Trigger type
#define HANTEK_DSO_TRIG_EDGE		0x00
#define HANTEK_DSO_TRIG_VIDEO		0x01
#define HANTEK_DSO_TRIG_PULSE		0x02
#define HANTEK_DSO_TRIG_SLOPE		0x03
#define HANTEK_DSO_TRIG_OT		0x04
#define HANTEK_DSO_TRIG_ALT		0x05
// Trigger source 
#define HANTEK_DSO_TRIG_CH1		0x00
#define HANTEK_DSO_TRIG_CH2		0x01
#define HANTEK_DSO_TRIG_EXT		0x02
#define HANTEK_DSO_TRIG_EXT5		0x03
#define HANTEK_DSO_TRIG_AC50		0x04
// Trigger mode
#define HANTEK_DSO_TRIG_MODE_AUTO	0x00
#define HANTEK_DSO_TRIG_MODE_NORM	0x01
// Trigger coupling
#define HANTEK_DSO_TRIG_DC		0x00
#define HANTEK_DSO_TRIG_AC		0x01
#define HANTEK_DSO_TRIG_NR		0x02
#define HANTEK_DSO_TRIG_HR		0x03
#define HANTEK_DSO_TRIG_LR		0x04


/*
 *	Struct for communication with Hantek DSOs
 */

union HantekDSO_settings {
	struct {
		// Channel 1 vertical settings
		uint8_t CH1_on;
		uint8_t CH1_volt_per_div;
		uint8_t CH1_coupling;
		uint8_t CH1_20mhz_filter;
		uint8_t CH1_tuning_type;
		uint8_t CH1_probe;
		uint8_t CH1_phase;
		uint8_t CH1_volt_per_div_fine;
		uint16_t CH1_position;	// First byte is upper byte, second is lower byte
		// Channel 2 vertical settings
		uint8_t CH2_on;
		uint8_t CH2_volt_per_div;
		uint8_t CH2_coupling;
		uint8_t CH2_20mhz_filter;
		uint8_t CH2_tuning_type;
		uint8_t CH2_probe;
		uint8_t CH2_phase;
		uint8_t CH2_volt_per_div_fine;
		uint16_t CH2_position;
		// Trigger status
		uint8_t trig_state;
		uint8_t trig_type;
		uint8_t trig_src;
		uint8_t trig_mode;
		uint8_t trig_coup;
		uint16_t trig_vpos;	// First byte is lower byte, second is upper byte
		uint8_t TRG_dummy[132];
		// Horizontal settings
		uint8_t timebase;
		uint8_t window_timebase;
		uint8_t window_state;
		uint64_t trigger_delay;	// Tatsächlich in 50ps Schritten

	} sysData;
	uint8_t raw[208];	// 213 bytes - 5 bytes (header + checksum) = 208 bytes of settings (according to readIn...)
};

class HantekDSO_5000P {

public:
	HantekDSO_5000P(rUSB_interface *_port) throw(HantekDSO_exception);
	~HantekDSO_5000P();

	// Beep!
	void beep() throw(rUSB_exception, HantekDSO_exception);

	// Lock/unlock control panel
	void lockPanel() throw(rUSB_exception, HantekDSO_exception);
	void unlockPanel() throw(rUSB_exception, HantekDSO_exception);

	// Start/stop acquisition
	void startDAQ() throw(rUSB_exception, HantekDSO_exception);
	void stopDAQ() throw(rUSB_exception, HantekDSO_exception);
	void keyPress(uint8_t _key) throw(rUSB_exception, HantekDSO_exception);

	// Get settings for readout
	void getSettings() throw(rUSB_exception, HantekDSO_exception);
	// Getter method for debugging...
	HantekDSO_settings Settings();
	// Apply settings to scope
	void applySettings() throw(rUSB_exception, HantekDSO_exception);
	void applySettings(HantekDSO_settings _settings) throw(rUSB_exception, HantekDSO_exception);
	uint8_t getTriggerState() throw(rUSB_exception, HantekDSO_exception);

	// Switch channel on/off
	void enableCH(int _chNo, bool _on_off);
	// Set channel coupling
	void setCoupling(int _chNo, uint8_t _vert_coup);
	// Change Volts/Div for given channel
	void setVertical(int _chNo, uint8_t _vert_VB);
	// Change probe attenuation for given channel
	void setAttenuation(int _chNo, uint8_t _vert_probe);

	void setTriggerMode(uint8_t _mode);
	void setTriggerLevel(int _trig_pos);

	// Apply vertical offset
	void setVertPos(int _chNo, int _pos);
	// Change seconds/Div for given channel
	void setTimebase(uint8_t _timebase);
	// Apply trigger delay
	void setTriggerDelay(long long int _delay);

	// Read one sample from scope
	int readSampleData(uint8_t _channel, double *_volts, double *_time, int _len, int _delay_us = 0) throw(rUSB_exception, HantekDSO_exception);

	// Flush read buffer
	void flushBuffer();

private:
	// Send request to scope
	int sendOut(uint8_t _marker, uint8_t _cmd, uint8_t *_data, int _len) throw(rUSB_exception);

	// Read data from scope, returns bytes received
	int readIn(uint8_t *_data, int _max_len) throw(rUSB_exception, HantekDSO_exception);

	// Issue a request and check return data
	void issueRequest(uint8_t _marker, uint8_t _cmd, uint8_t *_dataOut, int _lenOut, uint8_t *_dataIn, int _lenIn) throw(rUSB_exception, HantekDSO_exception);
	void issueRequest(uint8_t _marker, uint8_t _cmd, uint8_t *_data, int _len) throw(rUSB_exception, HantekDSO_exception);

	// Returns volts per division
	float getVoltsPerDiv(uint8_t _vb);
	// Returns probe attenuation
	double getAttenuation(uint8_t _att);
	// Returns seconds per division
	double getSecPerDiv(uint8_t _tb);

	rUSB_interface *__usb_port;
	int __ioInterfaceNo;
	uint8_t __ep_in_addr, __ep_out_addr; 
	
	// Channel settings
	HantekDSO_settings __DSOsettings;
	double __voltsPerDiv[HANTEK_DSO_N_CHANNELS], __probe_attenuation[HANTEK_DSO_N_CHANNELS], __secondsPerDiv;
	bool __channelEnable[HANTEK_DSO_N_CHANNELS];
	int16_t __channelOffset[HANTEK_DSO_N_CHANNELS];
};

#endif
