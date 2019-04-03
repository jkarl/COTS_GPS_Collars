#include <NMEAGPS.h>
NMEAGPS GPS;
gps_fix fix;
#include <SD.h>

#define LED_PIN (5)
#define RXPin (4)
#define TXPin (3)
#define LED1 (5)
#define LED2 (6)
#define GPSPOWER (7)
#define SDCHIPSELECT (8)
#define GREENLED (LED1)
#define REDLED (LED2)
#include <NeoSWSerial.h>
#define ARDUINO_GPS_RX 3 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 4 // GPS RX, Arduino TX pin
NeoSWSerial gpsPort(ARDUINO_GPS_TX, ARDUINO_GPS_RX);
//#define gpsPort Serial  // Alternatively, use Serial1 on the Leonardo, Mega or Due
#define GPS_BAUD 9600    // GPS module baud rate. GP3906 defaults to 9600.

      bool          waitingForFix = false;
const unsigned long GPS_TIMEOUT   = 120000; // 2 minutes
      unsigned long gpsStart      = 0;

const int           GPSpower      = 7;
bool turnGPSoff = false;
File dataFile;

void setup() {
  Serial.begin(9600);
  pinMode(GPSpower, OUTPUT);
  gpsPort.begin(GPS_BAUD);

  if (!SD.begin(SDCHIPSELECT)) // see if the card is present and can be initialized, also sets the object to hold onto that chip select pin
  {  
    //Serial.println("SD fail"); //Tell PC, can be commented out
    //Serial.println("Halting...");
    digitalWrite(GREENLED,LOW); 
    digitalWrite(REDLED,HIGH); 
    SD.end();
    while(1);
  }
  
}


void loop() {
  bool turnGPSoff = false;
  
  // Is a GPS fix available?
  if (GPS.available( gpsPort )) {
    fix = GPS.read();

    if (waitingForFix) {
      printGPSInfo();
      Serial.println( millis() - gpsStart ); // DEBUG

      if (fix.valid.satellites && (fix.satellites > 6)) {// I dont trust this
        waitingForFix = false;
        turnGPSoff    = true;
      }
    }
  }

  // Have we waited too long for a GPS fix?
 // if (waitingForFix && (millis() - gpsStart > GPS_TIMEOUT)) {
 //   waitingForFix = false;
 //   turnGPSoff    = true;
  //}

  if (turnGPSoff) {
    digitalWrite(GPSpower, LOW);
    Serial.println( F("GPS Toggled") );
    delay(1000*30);
    digitalWrite(GPSpower,HIGH);
    waitingForFix = true;
    turnGPSoff    = false;
  }

} // loop


void printGPSInfo()
{
  if (fix.valid.location){
  if (fix.valid.altitude){
  if (fix.valid.heading){
  if (fix.valid.speed){
  if (fix.valid.date){
  if (fix.valid.time){
  if (fix.valid.satellites){
  {
    dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
    if (dataFile)
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
    waitingForFix = false;
    turnGPSoff    = true;
  }}}}}}}}
}
