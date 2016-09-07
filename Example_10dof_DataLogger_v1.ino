#include <Wire.h>               //I2C library
#include <SPI.h>                //SPI library
#include <SD.h>                 //SD Card library
#include "RTClib.h"             //Real Time Clock library
#include <Adafruit_Sensor.h>    //Adafruit Sensor libary
#include <Adafruit_LSM303_U.h>  //Accel and Mag libary
#include <Adafruit_BMP085_U.h>  //Prsure Tempeture and Altitude libary
#include <Adafruit_L3GD20_U.h>  //Gyro libary



//File Name for wirghting to the SD card.
char filename[] = "DATA00.CSV";
File dataFile;
//For most shilds this is pin 10 check documentation to know what pin to use.
const int chipSelect = 10;

//Time interval in seconds.
float interval = 5;

//Decalre Names for each component.
RTC_DS1307 RTC;
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

void setup() {
  //Initialize Serial
  Serial.begin(9600);
  //Initialize I2C
  Wire.begin();
  //Initialize RTC
  RTC.begin();
  if (! RTC.isrunning()) {
    // gets time and date form your computer.
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  //Initialize 10-DOF
  Serial.println("Initializing 10-DOF...");
  if (!accel.begin() || !mag.begin() || !bmp.begin()) {
    Serial.println("Ooops, 10-DOF not detected ... Check your wiring!");
  }
  else {
    Serial.println("10-DOF Initialized.");
  }

  //Initialize SD
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  else {
    Serial.println("Card Initialized.");
  }

  //Find a unique file name.
  //This can be comented out if you want to wright to the same file.
  for (uint8_t i = 0; i < 100; i++) {
    //4 and 5 are the locations of the 0s inside of the filename.
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      dataFile = SD.open(filename, FILE_WRITE);
      //make a header at the top of the file.
      dataFile.println("Date, Time, AX, AY, AZ, GX, GY, GZ, MX, MY, MZ, ALT");
      dataFile.close();
      break;  // leave the loop!
    }
  }
  //print out the Filename.
  Serial.print("Wrighting to "); Serial.println(filename);


}

void loop() {

  //Open The SD File
  dataFile = SD.open(filename, FILE_WRITE);

  //Check to see if File is open
  if (! dataFile) {
    Serial.print("could not Open "); Serial.println(filename);
  }


  //Time And Date to Serial
  DateTime now = RTC.now();
  Serial.print("Date/Time ");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(", ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(",");

  //Time And Date to SD
  dataFile.print(now.month(), DEC);
  dataFile.print("/");
  dataFile.print(now.day(), DEC);
  dataFile.print(", ");
  dataFile.print(now.hour(), DEC);
  dataFile.print(":");
  dataFile.print(now.minute(), DEC);
  dataFile.print(":");
  dataFile.print(now.second(), DEC);
  dataFile.print(",");

  //Reseve Data form 10-DOF
  sensors_event_t event;
  accel.getEvent(&event);

  //acelration X
  Serial.print(" AX: ");
  Serial.print(event.acceleration.x);
  Serial.print(",");
  dataFile.print(event.acceleration.x);
  dataFile.print(",");

  //acelration Y
  Serial.print(" AY: ");
  Serial.print(event.acceleration.y);
  Serial.print(",");
  dataFile.print(event.acceleration.y);
  dataFile.print(",");

  //acelration Z
  Serial.print(" AZ: ");
  Serial.print(event.acceleration.z);
  Serial.print(",");
  dataFile.print(event.acceleration.z);
  dataFile.print(",");

  //Get Gyro Data
  gyro.getEvent(&event);

  //Gyro X
  Serial.print(" GX: ");
  Serial.print(event.gyro.x);
  Serial.print(",");
  dataFile.print(event.gyro.x);
  dataFile.print(",");

  //Gyro Y
  Serial.print(" GY: ");
  Serial.print(event.gyro.y);
  Serial.print(",");
  dataFile.print(event.gyro.y);
  dataFile.print(",");

  //Gyro Z
  Serial.print(" GZ: ");
  Serial.print(event.gyro.z);
  Serial.print(",");
  dataFile.print(event.gyro.z);
  dataFile.print(",");

  //Get Mag Data
  mag.getEvent(&event);

  //Mag x
  Serial.print(" MX: ");
  Serial.print(event.magnetic.x);
  Serial.print(",");
  dataFile.print(event.magnetic.x);
  dataFile.print(",");

  //Mag y
  Serial.print(" MY: ");
  Serial.print(event.magnetic.y);
  Serial.print(",");
  dataFile.print(event.magnetic.y);
  dataFile.print(",");

  //Mag z
  Serial.print(" MZ: ");
  Serial.print(event.magnetic.z);
  Serial.print(",");
  dataFile.print(event.magnetic.z);
  dataFile.print(",");

  //Get temp data
  bmp.getEvent(&event);
  Serial.print(" T: ");
  float Temp;
  bmp.getTemperature(&Temp);
  Serial.print(Temp);
  Serial.print(",");
  dataFile.print(Temp);
  dataFile.print(",");

  //Get Pressure data
  float pressure = event.pressure;
  Serial.print(" Hpa: ");
  Serial.print(pressure);
  Serial.print(",");
  dataFile.print(pressure);
  dataFile.print(",");

  //Get Pressure data
  Serial.print(" Altm: ");
  Serial.print(bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA,
                                      pressure,
                                      Temp));

  dataFile.print(bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA,
                                        pressure,
                                        Temp));


  //new line and close file
  Serial.println();
  dataFile.println();
  dataFile.close();

  //Convert seconds to milliseconds and Wait
  delay(interval * 1000);

}
