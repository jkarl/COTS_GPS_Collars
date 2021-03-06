/*
GPS collar
When the program begins it should check and SD card for settings.
It takes those settings, saves them, and then initializes the pins that are
the same on every board.  

It then enters an endless loop where it checks the GPS values, eventually 
saves them to the SD card, and then decides how long it should sleep.

While sleeping power consumption should be less than 1mA.  While searching for GPS
it should be between 50mA and 60mA.

Current work on the program is essentially searching for edge case errors that will
cause the units to stop working.




*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
//#include <avr/sleep.h>
//#include <avr/power.h>
//#include <avr/wdt.h>

#define LED_PIN (5)
#define RXPin (4)
#define TXPin (3)
#define LED1 (5)
#define LED2 (6)
#define GPSPOWER (7)
#define SDCHIPSELECT (8)
#define GREENLED (LED1)
#define REDLED (LED2)

uint32_t GPSBaud = 9600; //Set by the GPS manual.  The NEO M8N uses this baud rate.  Many use 4800 instead.

short int TIMEZONEADJ=0; //This value is ADDED to all time prints.
short int BEGINNIGHT=20,ENDNIGHT=23; //24 hour format for the window that begins longer sleep.  Suggested to keep this window small to avoid long sleeping twice.  Wouldnt go over 5.
short int ENDMONTH=13;
short int ENDDAY=40;
short int DESIREDHDOP=2500; //not counting the 10 multuplier - also set for indoor use.

short int LONGSLEEP=8; 
short int SHORTSLEEP=4;
short int sleeping=1;
int lastMin=-1;
int lastSec=-1;
// The TinyGPS++ object
TinyGPSPlus gps;
//SD file object
File dataFile;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//Function prototypes
void R1Begin();
void LoadSettings();
int NumFromSD();

/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
/*ISR(WDT_vect)
{
//This wakes the system, nothing actually needs to happen here

//It appears the interrupt flag doesn't need to be cleared, which is strange.
  MCUSR &= ~(1<<WDRF); 
//I've included it because it SHOULD need to be cleared.

}
*/

/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/

//void enterSleep(void)
//{
 // set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
 // sleep_enable();
  
  /* Now enter sleep mode. */
  //sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  //sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  //power_all_enable();
//}



/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Begin serial communication.  Calls a function
 *  to load settings.  Then it calls a setting that initializes the pins.
 *  This is a good function to edit to add functionality.
 *  
 *  This function also always runs once at every start up.
 *
 ***************************************************/
void setup()
{
  //Serial.begin(4800);
  //Serial.println("Starting");
  R1Begin();
  //Serial.println("Board Initialized");
  LoadSettings();
  //Serial.println("Settings Loaded");
  //delay(500);
  //digitalWrite(LED1,LOW);
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
    digitalWrite(LED1,HIGH);
    digitalWrite(GPSPOWER,HIGH); 
    delay(500); 
    digitalWrite(GPSPOWER,HIGH); 
    delay(50);
    ss.begin(GPSBaud);
}



/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
    if (ss.available() > 0)//GPS should almost always be available
      gps.encode(ss.read());
    else
    {
      ss.end();
      digitalWrite(GPSPOWER,HIGH);
      digitalWrite(REDLED,HIGH);
      delay(100);
      ss.begin(GPSBaud);
      digitalWrite(REDLED,LOW);
      delay(1000);
    }
      if(gps.location.isValid())
      {
        if(gps.hdop.isValid()&&gps.hdop.value()<DESIREDHDOP)
        {
          if(gps.date.isValid())
          {
            if(gps.time.isValid()&&gps.time.isUpdated())//the && time prevents double writtings
            {

/************VALID NUMBERS DETECTED, CHECK DATE****************/
                if(gps.date.month()==ENDMONTH&&gps.date.day()>=ENDDAY)//end of tests, wait forever.
                {
                  while(1)
                  {
                    //enterSleep();
                    delay(2950);
                    digitalWrite(LED2,HIGH);
                    delay(50);
                    digitalWrite(LED2,LOW);
                  }
                }

                
/**********NUMBERS CONVERTERED WRITE TO SD**********************/


              dataFile = SD.open("gpslog.csv", FILE_WRITE); //open SD
              if (dataFile)
              {
                //Print to SD
                dataFile.print(String(gps.date.year())  +",");
                dataFile.print(String(gps.date.month())+",");
                dataFile.print(String(gps.date.day())+",");
     
                dataFile.print(String(gps.time.hour())+",");
                dataFile.print(String(gps.time.minute())+",");
                dataFile.print(String(gps.time.second())+",");

                dataFile.print(String(gps.hdop.value())+",");
                dataFile.print(String(gps.satellites.value())+",");

                dataFile.print(gps.location.lat(),10);dataFile.print(",");
                dataFile.println(gps.location.lng(),10);// Printing the commas by themselves allows me to force the length of the
                                                      // float printed both to the terminal, and to the SD card.
               delay(100);
                dataFile.close();//Close SD
                
                //blink to indicate reading
                digitalWrite(LED1,LOW);
                delay(100);
                digitalWrite(LED1,HIGH);
                delay(100);
                digitalWrite(LED1,LOW);
                //if(Serialprinting)
                //{
                Serial.println("Wrote this to SD card:");
                Serial.print(String(gps.date.year())+",");
                Serial.print(String(gps.date.month())+",");
                Serial.print(String(gps.date.day())+",");

                Serial.print(String(gps.time.hour())+",");
                Serial.print(String(gps.time.minute())+",");
                Serial.print(String(gps.time.second())+",");

                Serial.print(String(gps.hdop.value())+",");
                Serial.print(String(gps.satellites.value())+",");

                Serial.print(gps.location.lat(),8);Serial.print(",");
                Serial.println(gps.location.lng(),8); // Printing the commas by themselves allows me to force the length of the
                
                //}                     // float printed both to the terminal, and to the SD card.
                                                      
              }
              else
              {  
                //Serial.println("error opening SD card file to save data.");
                digitalWrite(LED2,HIGH);
                delay(500);
                digitalWrite(LED2,LOW);
                dataFile.close();
              }

/**********************POWER OFF ADD ONS**********************/

                ss.end(); //Required. WTD will not work properly otherwise.
                delay(50);//stability
                digitalWrite(GPSPOWER,LOW);//shut down gps
                digitalWrite(LED1,LOW);//indicate shut down
                delay(50);//stability

/**********************HOW LONG WILL THE DEVICE SLEEP**********************/
              //Serial.println("Sleeping...");
              //Serial.end();
              sleeping=1;
              if(gps.time.hour()>=(int)BEGINNIGHT&&gps.time.hour()<(int)ENDNIGHT)//Night Time detected
              {      
                for(int sec=0, minutes=0, hours=0;hours<LONGSLEEP;sec+=8) //Actual waiting happens here
                {
                  //enterSleep();
                  delay(8000);
                  if(sec>=60)
                  {
                    minutes++;
                    sec=0;
                  }
                  if(minutes>=60)
                  {
                    hours++;
                    minutes=0;
                  }
                }
              }
              else //Night time is not detected
              {
                for(int sec=0, minutes=0;minutes<SHORTSLEEP;sec+=8) //Actual waiting happens here
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
              
              digitalWrite(LED1,HIGH);
              digitalWrite(GPSPOWER,HIGH); 
              delay(500); 
              digitalWrite(GPSPOWER,HIGH); 
              delay(50);
              ss.begin(GPSBaud);
            }
          }
        }
      }
}


/***************************************************
 *  Name:        R1Begin
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Initializes pins specific to the board.  Also sets up the
 *  watch dog timer.
 *
 ***************************************************/
void R1Begin()
{
  /*** initialize outputs ***/
  pinMode(GPSPOWER,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  

  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  //MCUSR &= ~(1<<WDRF); 
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  //WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  //WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  //WDTCSR |= _BV(WDIE);
  
  //Serial.println("Initialisation complete.");
  delay(50); //Allow for serial print to complete.
  //digitalWrite(LED1,LOW);
}


/***************************************************
 *  Name:        LoadSettings()
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Function opens SD card, then calls another function to read in.
 *  This is where the serial output happens to confirm that the settings are properly
 *  being read.
 *
 ***************************************************/

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
    TIMEZONEADJ=NumFromSD();
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
    DESIREDHDOP=NumFromSD();
    //Serial.print("HDOP: ");
    //Serial.println(DESIREDHDOP);
    GPSBaud=NumFromSD();
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


/***************************************************
 *  Name:        NumFromSD()
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Open SD card and read in.  The function
 *  ignores the first boxes in an excel sheet. It also skips over
 *  the new line.  It reads until it sees a new line, then takes what
 *  it assumes to be a number and puts it into the settings.
 *  
 *  To see if it's working, use the serial commands that are in the
 *  Load Settings function above.
 *
 ***************************************************/
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
