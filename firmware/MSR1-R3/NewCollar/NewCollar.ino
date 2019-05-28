#include <NMEAGPS.h>
#include <SD.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define LED_PIN (5)
#define RXPin (4)
#define TXPin (3)
#define LED1 (5)
#define LED2 (6)
#define GPSpower (7)
#define SDCHIPSELECT (8)
#define GREENLED (LED1)
#define REDLED (LED2)
#include <NeoSWSerial.h>
#define ARDUINO_GPS_RX 3 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 4 // GPS RX, Arduino TX pin
#define Second 1000

void LoadSettings();
void Blink(int pin);
void SystemInitialize();
void Sleep(int MinutesToSleep);
int printGPSInfo();
int NumFromSD();
void EnterSleep();

NMEAGPS GPS;
gps_fix fix;
NeoSWSerial gpsPort(ARDUINO_GPS_TX, ARDUINO_GPS_RX);
File dataFile;

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

ISR(WDT_vect)
{
//This wakes the system, nothing actually needs to happen here

//It appears the interrupt flag doesn't need to be cleared, which is strange.
  MCUSR &= ~(1<<WDRF); 
//I've included it because it SHOULD need to be cleared.

}



void setup() {
  SystemInitialize();
  //Serial.begin(9600);
  LoadSettings();
  digitalWrite(GPSpower,HIGH);
  delay(10);
  gpsPort.begin(GPS_BAUD);
  Blink(REDLED);
  Blink(GREENLED);
  digitalWrite(GREENLED,HIGH);
  digitalWrite(REDLED,HIGH);
  delay(50);
  Blink(REDLED);
  Blink(GREENLED);
  
}


void loop() {
  //digitalWrite(GREENLED,!digitalRead(GREENLED)); //heartbeat
    
  // Is a GPS fix available?*******************
  if (GPS.available( gpsPort )) 
  {
    fix = GPS.read();
    Blink(GREENLED);
    if (waitingForFix) 
    {
      //Blink(REDLED);
        if (fix.valid.location&&fix.valid.date&&fix.valid.time)
        {
          gpsPort.end();
          digitalWrite(GPSpower, LOW);
          printGPSInfo();//Attempt to print
          waitingForFix = 0;
          turnGPSoff    = 1;
        }
        else
        {
          Blink(REDLED);
        }
      //Serial.println( millis() - GPS_TIME ); // DEBUG
    }
  }
  //***************************************

  
  // Have we waited too long for a GPS fix?
  if (waitingForFix && (millis() - GPS_TIME > GPS_TIMEOUT*Second)) 
  {
    waitingForFix = 0;
    turnGPSoff    = 1;
    Blink(REDLED);
    Blink(REDLED);
    dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
    while(!dataFile)
    {
      SD.end();
      delay(200);
      SD.begin(SDCHIPSELECT);
      dataFile = SD.open("gpslog.csv", FILE_WRITE);
    }
    if (dataFile)
    {
    dataFile.println("0,0,0,0,0,0,0,0,0"); //print a blank
    dataFile.close();
    }
    
 }

//******************************************
 
 //Sleep 
  if (turnGPSoff) 
  {

    //digitalWrite(REDLED,HIGH);
    /*if((int)fix.dateTime.month==ENDMONTH) Removed for now, plan to re-add after testing confirms this is not the error
      if((int)fix.dateTime.day==ENDDAY)
      {
        while(1)
        {
          //sleep forever
          Blink(GREENLED);
          Blink(REDLED);
          Sleep(1);
        }
      }*/
      
    if((int)fix.dateTime.hours>=BEGINNIGHT&&(int)fix.dateTime.hours<=ENDNIGHT)
    {
      Sleep(60*LONGSLEEP);
    }
    else
    {
      Sleep(SHORTSLEEP);
    }
    
    digitalWrite(GPSpower,HIGH);
    gpsPort.begin(GPS_BAUD);
    Blink(GREENLED);
     
    waitingForFix = 1;
    turnGPSoff    = 0;
    GPS_TIME=millis();
  }


  //******************************
  delay(100);
} // loop


int printGPSInfo()
{

    //Serial.println(fix.dateTime.hours);
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

void Sleep(int MinutesToSleep)
{
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

void SystemInitialize()
{
  pinMode(GPSpower, OUTPUT);
  pinMode(REDLED,OUTPUT);
  pinMode(GREENLED,OUTPUT);
  if (!SD.begin(SDCHIPSELECT)) // see if the card is present and can be initialized, also sets the object to hold onto that chip select pin
  {  
    //Serial.println("SD fail"); //Tell PC, can be commented out
    //Serial.println("Halting...");
    digitalWrite(GREENLED,LOW); 
    digitalWrite(REDLED,HIGH); 
    SD.end();
    while(!SD.begin(SDCHIPSELECT))
    {
      SD.end();
    }
    LoadSettings();
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
      delay(200);
      SD.begin(SDCHIPSELECT);
      dataFile = SD.open("settings.csv", FILE_WRITE);
      //Blink(REDLED);
      Error++;
    }
  if(dataFile = SD.open("settings.csv", FILE_READ))
  {
  //Serial.println("Opening settings");
    //TIMEZONEADJ=NumFromSD();
    //Serial.print("Time Zone: ");
    //Serial.println(TIMEZONEADJ);
    SHORTSLEEP=NumFromSD();
    //Serial.print("Minute Sleep: ");
    //Serial.println(SHORTSLEEP);
    LONGSLEEP=NumFromSD();
    //Serial.print("Hour Sleep: ");
    //Serial.println(LONGSLEEP);
    BEGINNIGHT=NumFromSD();
    //Serial.print("Night (24-hour): ");
    //Serial.println(BEGINNIGHT);
    ENDNIGHT=NumFromSD();
    //Serial.print("Day (24-hour): ");
    //Serial.println(ENDNIGHT);
    //DESIREDHDOP=NumFromSD();
    //Serial.print("HDOP: ");
    //Serial.println(DESIREDHDOP);
    GPS_BAUD=NumFromSD();
    //Serial.print("GPS baud rate: ");
    //Serial.println(GPS_BAUD);
    //Serialprinting=NumFromSD();
    ENDMONTH=NumFromSD();
    //Serial.print("End Month: ");
    ENDDAY=NumFromSD();
    GPS_TIMEOUT=NumFromSD();
  }
  else
  {
    //Serial.println("Settings not found, using default.");
  }
    dataFile.close();
  
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
void EnterSleep(void)
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
