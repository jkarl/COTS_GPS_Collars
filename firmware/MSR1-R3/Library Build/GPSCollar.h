
#ifndef GPSCollar_h
#define GPSCollar_h
#include "Arduino.h"

//needed for the library to function properly.
#include <NMEAGPS.h>
#include <SD.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <NeoSWSerial.h>

//hardware configuration
#define _LED_PIN (5)
#define _RXPin (4)
#define _TXPin (3)
#define _LED1 (5)
#define _LED2 (6)
#define _GPSpower (7)
#define _SDCHIPSELECT (8)
#define _GREENLED (_LED1)
#define _REDLED (_LED2)
#define _ARDUINOGPSRX (3) // GPS TX, Arduino RX pin
#define _ARDUINOGPSTX (4) // GPS RX, Arduino TX pin
#define _Second 1000






class GPSCollar
{
	public:
		GPSCollar(NMEAGPS *GPS,gps_fix *fix,NeoSWSerial gpsPort(int,int),File *dataFile);//initialize the system
		
		//basic functions
		void Blink(int pin); // turns off the pin, then on, then off again.
		void WaitSleep(int MinutesToSleep); //uses a simple loop to wait an amount of time. Higher power consumption.
		int CollarSD(); //returns the SD card pin number, allowing users access to the SD card in a readable manner.
		
		
		
		void WDTInit()//gets the WDT ready to use
		{
			MCUSR &= ~(1<<WDRF);
			WDTCSR |= (1<<WDCE) | (1<<WDE);
			WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
			WDTCSR |= _BV(WDIE);
			delay(50);
		}
		/*
			The low power option to sleeping the system.  Currently not 100% stable, and some odd issues have occured.
			If using this make sure to initialize the WTD, using the WTD initialize function, otherwise it may never
			wake up.
		*/
		void WDTSleep(int MinutesToSleep);  // Uses the watch dog timer to wait an amount of time.  
		
		
		
		/*
			After locking on and getting valid data (validity is NOT checked in this function) this fucntion
			can be used to dump multiple pieces of data in a single line into a csv file named gpslog.  The format is as follows:
			
			year,month,day,hour,minute,second,satellite,lat(10),long(10) -line end
			
			it returns 1 on a failure, and does so quitely.  Users will need to decide what happens with a failure.
		*/
		int DumpGPSInfo(); 
		
		
		/*
			Load settings attempts to read from an SD card (already defined in the library) from a file called
			"settings.csv".  It returns 0 on success, but will not check for valid data.  It returns 1 if it cannot open
			a file named "settings.csv".  It will fail quitely here, and the user needs to create an event to happen
			in the event of a failure.
		*/
		int LoadSettings();
		
		/*
			Checks to see if there is GPS data available.  If there is, it grabs it and puts it in the fix variable.
			It can be written with the DumpGPSInfo() option.
		*/
		int AttemptGPS();
	private:	
		//In software configurables, changed by LoadSettings.
		int _SHORTSLEEP=9;
		int _LONGSLEEP=8;
		int _BEGINNIGHT=25;
		int _ENDNIGHT=25;
		int _GPS_BAUD=9600;
		int _ENDMONTH=-1;
	    int _ENDDAY=-1;
		int _waitingForFix = 1;
		double _GPS_TIMEOUT   = 90; // 1.5 minutes
		double _GPS_TIME      = 0;
		int _turnGPSoff = 0;
		
		NMEAGPS *_GPS;
		gps_fix *_fix;
		NeoSWSerial *_gpsPort(int,int); //possible source of error here
		File *_dataFile;
		
		//Hidden functions
		int NumFromSD(); // a hidden function that helps read our settings file.

};

#endif