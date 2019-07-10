#include "Arduino.h"
#include "Collar.h"

Collar::Collar()
{
  pinMode(GPSpower, OUTPUT);
  pinMode(REDLED,OUTPUT);
  pinMode(GREENLED,OUTPUT);
  if (!SD.begin(SDCHIPSELECT)) // see if the card is present and can be initialized, also sets the object to hold onto that chip select pin
  {  
    digitalWrite(GREENLED,LOW); 
    digitalWrite(REDLED,HIGH); 
    SD.end();
    while(1)
    {
      digitalWrite(REDLED,HIGH);
    }
   }
  digitalWrite(REDLED,LOW);
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  WDTCSR |= _BV(WDIE);
  delay(50);
}

void LoadSettings()
{
  dataFile = SD.open("settings.csv", FILE_WRITE);
  int Error=0;
  while(!dataFile&&Error<10)
    {
      SD.end();
      delay(500);
      SD.begin(SDCHIPSELECT);
      dataFile = SD.open("settings.csv", FILE_WRITE);
      Blink(REDLED);
      Error++;
    }
  if(dataFile = SD.open("settings.csv", FILE_READ))
  {
    SHORTSLEEP=NumFromSD();
    LONGSLEEP=NumFromSD();
    BEGINNIGHT=NumFromSD();
    ENDNIGHT=NumFromSD();
    GPS_BAUD=NumFromSD();
    ENDMONTH=NumFromSD();
    ENDDAY=NumFromSD();
    GPS_TIMEOUT=NumFromSD();
  }
  else
  {
  }
    dataFile.close();
}
void Blink(int pin)
{
  digitalWrite(pin,LOW);
  delay(100);
  digitalWrite(pin,HIGH);
  delay(100);
  digitalWrite(pin,LOW);
  delay(100);
}
void Sleep(int MinutesToSleep)
{
  interrupts();
	for(int sec=0, minutes=0;minutes<MinutesToSleep;sec+=8) //Actual waiting happens here
				{
				  EnterSleep();
				  //delay(8000);
				  if(sec>=60)
				  {
					minutes++;
					sec=0;
				  }
				}
	noInterrupts();
}
int printGPSInfo()
{
	dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
    while(!dataFile)
    {
      SD.end();
      delay(200);
      SD.begin(SDCHIPSELECT);
      dataFile = SD.open("gpslog.csv", FILE_WRITE);
      //Blink(REDLED);
    }
    if (dataFile)
    {
      Blink(GREENLED); 
      dataFile.print(String(fix.dateTime.year));
      dataFile.print(",");
      dataFile.print(String(fix.dateTime.month));
      dataFile.print(",");
      dataFile.print(String(fix.dateTime.date));
      dataFile.print(",");
      dataFile.print(String(fix.dateTime.hours));
      dataFile.print(",");
      dataFile.print(String(fix.dateTime.minutes));
      dataFile.print(",");
      dataFile.print(String(fix.dateTime.seconds));
      dataFile.print(",");


      dataFile.print(String(fix.valid.satellites));
      dataFile.print(",");
      dataFile.print(fix.latitude(),10);dataFile.print(",");
      dataFile.println(fix.longitude(),10);
      dataFile.close();
      return 1;
    }
  else
  {
    digitalWrite(REDLED,HIGH);
    digitalWrite(GREENLED,HIGH);
    delay(500);
    digitalWrite(REDLED,LOW);
    digitalWrite(GREENLED,LOW);
    return 0;
  }
}
int NumFromSD()
{
	int ToReturn=0;
  int buf;
  int num[4];
  int i=0,neg=0,j=0;
  while(dataFile.read()!=','); //read until ,
  do//begin extracting numbers
  {
    buf=dataFile.read(); //get char
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
void EnterSleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}