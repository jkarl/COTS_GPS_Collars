#include <GPSCollar.h>

int GPSCollar::printGPSInfo()
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

void GPSCollar::WaitSleep(int MinutesToSleep)
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

void GPSCollar::WDTSleep(int MinutesToSleep)
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


void GPSCollar::Blink(int pin)
{
  digitalWrite(pin,LOW);
  delay(100);
  digitalWrite(pin,HIGH);
  delay(100);
  digitalWrite(pin,LOW);
  delay(100);

}

void GPSCollar::LoadSettings()
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

int GPSCollar::NumFromSD()
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
void GPSCollar::EnterSleep(void)
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