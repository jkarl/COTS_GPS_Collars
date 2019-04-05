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
#define GPS_BAUD 9600    // GPS module baud rate. GP3906 defaults to 9600.
#define Second 1000


NMEAGPS GPS;
gps_fix fix;
NeoSWSerial gpsPort(ARDUINO_GPS_TX, ARDUINO_GPS_RX);

      bool          waitingForFix = true;
const unsigned long GPS_TIMEOUT   = 60000; // 2 minutes
      unsigned long GPS_TIME      = 0;
      bool turnGPSoff = false;
      File dataFile;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting....");
  SystemInitialize();
  digitalWrite(GPSpower,HIGH);
  gpsPort.begin(GPS_BAUD);

  
  
}


void loop() {
  digitalWrite(GREENLED,!digitalRead(GREENLED)); //heartbeat
  
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
 // if (waitingForFix && (millis() - gpsStart > GPS_TIMEOUT)) {
 //   waitingForFix = false;
 //   turnGPSoff    = true;
  //}
//******************************************
 
 //Sleep 
  if (turnGPSoff) 
  {
    digitalWrite(GPSpower, LOW);
    digitalWrite(GREENLED,LOW);
    digitalWrite(REDLED,HIGH);
    Sleep(1);
    digitalWrite(REDLED,LOW);
    digitalWrite(GPSpower,HIGH);
    waitingForFix = true;
    turnGPSoff    = false;
    delay(15*Second); 
  }
  //******************************

} // loop


bool printGPSInfo()
{
  if (fix.valid.location){
  if (fix.valid.altitude){
  if (fix.valid.heading){
  if (fix.valid.speed){
  if (fix.valid.date){
  if (fix.valid.time){
  if (fix.valid.satellites&& (fix.satellites > 4)){
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
    return true;
    }
    else
    {
      Blink(REDLED);
      Blink(REDLED);
    }
  }}}}}}}}
  return false;
}

void Sleep(int Minutes)
{
  delay(Second*60*Minutes);
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
