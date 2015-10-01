// stdlib
#include <stdio.h>
#include <unistd.h>

// Runic library
#include <usb/rUSB_interface.h>
#include <usb/Hantek/HantekDSO_5000P.h>

int main(){

	rUSB_interface usb_dso(HANTEK_DSO_5072P_VID, HANTEK_DSO_5072P_PID);
	usb_dso.printInfo();

	HantekDSO_5000P dso(&usb_dso);

	double data[13000], time[13000]; 
	int sampleLen = -1;

	dso.beep();
	dso.lockPanel();
	dso.getSettings();
	dso.unlockPanel();

	// Wait until there is data to be read
	while (sampleLen == HANTEK_DSO_NODATA){
		sampleLen = dso.readSampleData(HANTEK_DSO_CH1, data, time, sizeof(data));
	}

	// Print all data
	for (int i = 0; i < sampleLen; i++){
		printf ("(%f, %f)\n", time[i], data[i]);
	}
	printf("%i bytes in total.\n", sampleLen);

	return 0;
}


