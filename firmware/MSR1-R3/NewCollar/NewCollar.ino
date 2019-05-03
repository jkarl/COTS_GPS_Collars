//Software Version 1.0

#include <NMEAGPS.h>
#include <SD.h>

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


NMEAGPS GPS;
gps_fix fix;
NeoSWSerial gpsPort(ARDUINO_GPS_TX, ARDUINO_GPS_RX);
      int SHORTSLEEP=9;
      int LONGSLEEP=8;
      int BEGINNIGHT=25;
      int ENDNIGHT=25;
      int GPS_BAUD=9600;
      int ENDMONTH=-1;
      int ENDDAY=-1;
      bool          waitingForFix = true;
const unsigned long GPS_TIMEOUT   = 60000;
      unsigned long GPS_TIME      = 0;
      bool turnGPSoff = false;
      File dataFile;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting....");
  SystemInitialize();
  //LoadSettings();
  digitalWrite(GPSpower,HIGH);
  gpsPort.begin(GPS_BAUD);
  Blink(GREENLED);
  
  
}


void loop() {
  //digitalWrite(GREENLED,!digitalRead(GREENLED)); //heartbeat
  
  // Is a GPS fix available?*******************
  if (GPS.available( gpsPort )) 
  {
    fix = GPS.read();
    if (waitingForFix) 
    {
      if(printGPSInfo())//Attempt to print
        {
          waitingForFix = false;
          turnGPSoff    = true;
        }
      Serial.println( millis() - GPS_TIME ); // DEBUG
    }
  }
  //***************************************

  
  // Have we waited too long for a GPS fix?
  if (waitingForFix && (millis() - GPS_TIME > GPS_TIMEOUT)) {
    waitingForFix = false;
    turnGPSoff    = true;
 }
//******************************************
 
 //Sleep 
  if (turnGPSoff) 
  {
    digitalWrite(GPSpower, LOW);
    digitalWrite(GREENLED,LOW);
    //digitalWrite(REDLED,HIGH);
    if((int)fix.dateTime.month==ENDMONTH)
      if((int)fix.dateTime.day==ENDDAY)
      {
        while(1)
        {
          //sleep forever
          Blink(GREENLED);
          Blink(REDLED);
          Sleep(1);
        }
      }
    if((int)fix.dateTime.hours>=BEGINNIGHT&&(int)fix.dateTime.hours<=ENDNIGHT)
    {
      Sleep(60*LONGSLEEP);
    }
    else
    {
      Sleep(SHORTSLEEP);
    }
    
    digitalWrite(REDLED,LOW);
    digitalWrite(GPSpower,HIGH);
    delay(45*Second);
    Blink(GREENLED);
    Blink(GREENLED); 
    waitingForFix = true;
    turnGPSoff    = false;
    GPS_TIME=millis();
  }
  //******************************

} // loop


bool printGPSInfo()
{
  if (fix.valid.location){
  if (fix.valid.date){
  if (fix.valid.time){
  {
    Serial.println(fix.dateTime.hours);
    dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
    if (dataFile)
    {
    dataFile.print(String(fix.dateTime.year)  +",");
    dataFile.print(String(fix.dateTime.month)+",");
    dataFile.print(String(fix.dateTime.date)+",");
     
    dataFile.print(String(fix.dateTime.hours)+",");
    dataFile.print(String(fix.dateTime.minutes)+",");
    dataFile.print(String(fix.dateTime.seconds)+",");


    dataFile.print(String(fix.valid.satellites)+",");

    dataFile.print(fix.latitude(),10);dataFile.print(",");
    dataFile.println(fix.longitude(),10);
    dataFile.close();
    Blink(GREENLED);
    Blink(GREENLED);
    Blink(GREENLED);

    return true;
    }
    else
    {
      //Blink(REDLED);
      //Blink(REDLED);
      return false;
    }
  }}}}
}

void Sleep(int MinutesToSleep)
{
   for(int sec=0, minutes=0;minutes<MinutesToSleep;sec+=8) //Actual waiting happens here
                {
                  //enterSleep();
                  delay(8000);
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
  delay(10);
  digitalWrite(pin,HIGH);
  delay(200);
  digitalWrite(pin,LOW);
  delay(200);
  digitalWrite(pin,HIGH); 
  delay(200);
  digitalWrite(pin,LOW);
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
    while(1)
    {
      Blink(REDLED);
    }
  }
}

void LoadSettings()
{
  if (!SD.begin(SDCHIPSELECT)) // see if the card is present and can be initialized, also sets the object to hold onto that chip select pin
  {  
    //Serial.println("SD fail"); //Tell PC, can be commented out
    //Serial.println("Halting...");
    digitalWrite(GREENLED,LOW); 
    digitalWrite(REDLED,HIGH); 
    SD.end();
    while(1);
  }
  if(dataFile = SD.open("settings.csv", FILE_READ))
  {
  //Serial.println("Opening settings");

    //Serial.println(F("Terminal for GPS Collar MS-R1"));
    //Serial.println(F("Created 12/19/2018"));
    //Serial.println(F("Last code update on 1/16/2018"));
    //Serial.println(F("Beginning Startup..."));
    //Serial.println(F("SD Card Detected"));
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
    //Serial.println(GPSBaud);
    //Serialprinting=NumFromSD();
    ENDMONTH=NumFromSD();
    //Serial.print("End Month: ");
    ENDDAY=NumFromSD();
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
