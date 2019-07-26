#include <GPSCollar.h>

//The interrupt used by WDT (needs to be here in a global area)
ISR(WDT_vect)
{
//This wakes the system, nothing actually needs to happen here

//It appears the interrupt flag doesn't need to be cleared, which is strange.
  MCUSR &= ~(1<<WDRF); 
//I've included it because it SHOULD need to be cleared.

}





GPSCollar::GPSCollar(NMEAGPS *GPS,gps_fix *fix,NeoSWSerial gpsPort(int,int),File *dataFile)
{
	_GPS=GPS;
	_fix=fix;
	_gpsPort=gpsPort;
	_dataFile=dataFile;
	pinMode(_GPSpower, OUTPUT);
	pinMode(_REDLED,OUTPUT);
	pinMode(_GREENLED,OUTPUT);
	if (!SD.begin(_SDCHIPSELECT)) // see if the card is present and can be initialized, also sets the object to hold onto that chip select pin
	{  
		digitalWrite(_GREENLED,LOW); 
		digitalWrite(_REDLED,HIGH); 
		SD.end();
   }
   else
   {
	   digitalWrite(_GREENLED,HIGH);
   }
}

int GPSCollar::AttemptGPS()
{
  if (_GPS.available( _gpsPort )) //is there data?
  {
    _fix = _GPS.read(); //if yes, take it
	if (_fix.valid.location&&_fix.valid.date&&_fix.valid.time) //is the data valid?
	{
		//fix should already have the valid data, and I don't want to touch the SD here.
		return 0;
	}
  }
  return 1;
}

int GPSCollar::DumpGPSInfo()
{
    _dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
	int error =0;
    while(!_dataFile&&error<10)
    {
      SD.end();
      delay(200);
      SD.begin(_SDCHIPSELECT);
      _dataFile = SD.open("gpslog.csv", FILE_WRITE);
      error++;
    }
    if (_dataFile)
    {
      _dataFile.print(String(_fix.dateTime.year));
      _dataFile.print(",");
      _dataFile.print(String(_fix.dateTime.month));
      _dataFile.print(",");
      _dataFile.print(String(_fix.dateTime.date));
      _dataFile.print(",");
      _dataFile.print(String(_fix.dateTime.hours));
      _dataFile.print(",");
      _dataFile.print(String(_fix.dateTime.minutes));
      _dataFile.print(",");
      _dataFile.print(String(_fix.dateTime.seconds));
      _dataFile.print(",");
      _dataFile.print(String(_fix.valid.satellites));
      _dataFile.print(",");
      _dataFile.print(_fix.latitude(),10);_dataFile.print(",");
      _dataFile.println(_fix.longitude(),10);
      _dataFile.close();
      return 0;
    }
  else
  {
    return 1;
  }
}

void GPSCollar::WaitSleep(int MinutesToSleep)
{
	delay(MinutesToSleep*60*_Second);
}

void GPSCollar::WDTSleep(int MinutesToSleep)
{
   for(int sec=0, minutes=0;minutes<MinutesToSleep;sec+=8) //Actual waiting happens here
                {
                    set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
					sleep_enable();
  
					/* Now enter sleep mode. */
					sleep_mode();
  
					/* The program will continue from here after the WDT timeout*/
					sleep_disable(); /* First thing to do is disable sleep. */
  
					/* Re-enable the peripherals. */
					power_all_enable(); 
					if(sec>=60)
					{
						minutes++;
						sec=0;
					}
                }
}


void GPSCollar::Blink(int pin)
{
  digitalWrite(pin,LOW);
  delay(100);
  digitalWrite(pin,HIGH);
  delay(100);
  digitalWrite(pin,LOW);
  delay(100);

}

int GPSCollar::LoadSettings()
{
  _dataFile = SD.open("settings.csv", FILE_WRITE);
  int Error=0;
  while(!_dataFile&&Error<10)
    {
      SD.end();
      delay(200);
      SD.begin(_SDCHIPSELECT);
      _dataFile = SD.open("settings.csv", FILE_WRITE);
      Error++;
    }
  if(_dataFile = SD.open("settings.csv", FILE_READ))
  {
    _SHORTSLEEP=NumFromSD();
    _LONGSLEEP=NumFromSD();
    _BEGINNIGHT=NumFromSD();
    _ENDNIGHT=NumFromSD();
    _GPS_BAUD=NumFromSD();
    _ENDMONTH=NumFromSD();
    _ENDDAY=NumFromSD();
    _GPS_TIMEOUT=NumFromSD();
	
	return 0;
  }
  else
  {
    //Multiple fails, give up
	return 1;
  }
    _dataFile.close();
  
}

int GPSCollar::NumFromSD()
{
  int ToReturn=0;
  int buf;
  int num[4];
  int i=0,neg=0,j=0;
  while(_dataFile.read()!=','); //read until ,
  do//begin extracting numbers
  {
    buf=_dataFile.read(); //get char
    if(buf!=13&&buf!='-'&&buf>=48&&buf<=57)
    {
    buf-=48; //convert char number to int number
    num[i]=buf; //store in array
      while(j<i)
      {
        num[j]=num[j]*10;
        j++;
      }
      j=0;
      i++;
    }
    else if(buf=='-')
    {
      neg=1;
    }
  } while(buf!=13);
  while(i>j)
  {
    ToReturn=ToReturn+num[j];
    j++;
  }
  if(neg)
    {
      ToReturn=ToReturn*-1;
    }
  return(ToReturn);
}

int GPSCollar::CollarSD()
{
	return _SDCHIPSELECT;
}