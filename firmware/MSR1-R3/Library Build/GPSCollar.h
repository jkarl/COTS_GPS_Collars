
#ifndef GPSCollar_h
#define GPSCollar_h
#include "Arduino.h"

class GPSCollar
{
	public:
		void Blink(int pin); // Simply turns on, then off an LED. Used as an indicator.
		void WaitSleep(int MinutesToSleep); //uses a simple loop to wait an amount of time. Higher power consumption.
		void WDTSleep(int MinutesToSleep);  // Uses the watch dog timer to wait an amount of time.  
		int printGPSInfo(); // Print information to an SD card. (needs to be updated to take in a string)
		void LoadSettings();// Read settings file from SD card.
		int CollarSD(); //returns the SD card pin number, allowing me to avoid passing the SD object into the library.
	private:
		//Actual hardware configuration here, should be constant between models.
		const int _LED_PIN = 5;
		const int _RXPin = 4;
		const int _TXPin = 3;
		const int _LED1 = 5;
		const int _LED2 = 6;
		const int _GPSpower = 7;
		const int _SDCHIPSELECT = _CollarSD;
		const int _GREENLED = _LED1;
		const int _REDLED = _LED2;
		const int _ARDUINO_GPS_RX = 3; // GPS TX, Arduino RX pin
		const int _ARDUINO_GPS_TX = 4;// GPS RX, Arduino TX pin
		const int _Second = 1000;
		
		//In software configurables, changed by LoadSettings.
		int SHORTSLEEP=9;
		int LONGSLEEP=8;
		int BEGINNIGHT=25;
		int ENDNIGHT=25;
		int GPS_BAUD=9600;
		int ENDMONTH=-1;
	    int ENDDAY=-1;
		int waitingForFix = 1;
		double GPS_TIMEOUT   = 90; // 1.5 minutes
		double GPS_TIME      = 0;
		int turnGPSoff = 0;
		
		//Hidden functions
		int NumFromSD(); // a hidden function that helps read our settings file.
		void EnterSleep();  //a hidden place holder function, to be removed.
}

#endif