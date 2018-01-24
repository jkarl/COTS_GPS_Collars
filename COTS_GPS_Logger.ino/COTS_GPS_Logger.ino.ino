#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   Code for creating a simple GPS data logger from a uBLOX drone GPS unit, Arduino Pro Mini,
   and a microSD module. This code uses the LowPower library to minimize power draw and obtain
   GPS readings only at specified intervals. 

   This code draws from the following source scripts:
   - https://www.arduino.cc/en/Tutorial/Datalogger
   - TinyGPS++ sample sketch by Mikal Hart
   - LowPower sample sketch
*/

// Set up GPS pins and baud rate
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// Initialize the TinyGPS++ object and serial connection to the GPS device
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// Set up the SD card module
const int chipSelect = 8;





//////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.

  // Check for GPS connected.
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  // Poll GPS unit, write result to SD card.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      //displayInfo();
      String GPSdata = GPSline();
      File dataFile = SD.open("gpslog.csv", FILE_WRITE);
      // if the file is available, write to it:
      if (dataFile) {
        Serial.println("Writting GPS data to SD card.")
        dataFile.println(GPSdata);
        dataFile.close();
        // print to the serial port too:
        Serial.println(GPSdata);
      } else {
        Serial.println("error opening SD card file.");
      }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// Function to poll the GPS unit and get the location, quality info, and date/time.
String GPSline()
{
  String gpsLat = "", gpsLon = "";
  String sats = "", HDOP = "";
  String gpsdate = "", gpstime = "";
  
  // Get GPS location fields or set NoData if invalid.
  if (gps.location.isValid())
  {
    gpsLat = gps.location.lat();
    gpsLon = gps.location.lng();
  } else {
    gpsLat = -9999.0;
    gpsLon = -9999.0;
  }

  // Get GPS signal quality info
  if (gps.satellites.isValid())
  {
    sats = gps.satellites.value();
    HDOP = gps.hdop.value();
  } else {
    sats = -9999;
    HDOP = -9999;
  }

  // Get GPS date and time
  if (gps.date.isValid())
  {
    gpsdate = gps.date.value();
    gpstime = gps.time.value();
  } else {
    gpsdate = -9999;
    gpstime = -9999;
  }

  // return the full GPS line
  return gpsLat+","+gpsLon+","+sats+","+HDOP+","+gpsdate+","+gpstime;
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Satellite Info: "));
  if (gps.satellites.isValid())
  {
    Serial.print(F("Satellites: "));
    Serial.print(gps.satellites.value());
    Serial.print(F(", HDOP: "));
    Serial.print(gps.hdop.value());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
