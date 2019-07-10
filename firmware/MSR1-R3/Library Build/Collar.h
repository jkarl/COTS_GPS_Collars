#ifndef Collar_h
#define Collar_h

#include "Arduino.h"


class Collar
{
public:
Collar();
void LoadSettings();
void Blink(int pin);
void Sleep(int MinutesToSleep);
int printGPSInfo();
int NumFromSD();
void EnterSleep();

private:
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
}

#endif