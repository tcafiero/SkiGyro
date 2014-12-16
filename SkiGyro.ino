
/***************************************************
  This is an example for the Adafruit Triple-Axis Gyro sensor

  Designed specifically to work with the Adafruit L3GD20 Breakout
  ----> https://www.adafruit.com/products/1032

  These sensors use I2C or SPI to communicate, 2 pins (I2C)
  or 4 pins (SPI) are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Kevin "KTOWN" Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <RFduinoBLE.h>
#include <Wire.h>
#include <stdio.h>

#include <Adafruit_L3GD20.h>
#define USE_I2C
// Comment this next line to use SPI
//#define USE_I2C

#ifdef USE_I2C
// The default constructor uses I2C
Adafruit_L3GD20 gyro;
#else
// To use SPI, you have to define the pins
#define GYRO_CS 4 // labeled CS
#define GYRO_DO 5 // labeled SA0
#define GYRO_DI 6  // labeled SDA
#define GYRO_CLK 7 // labeled SCL
Adafruit_L3GD20 gyro(GYRO_CS, GYRO_DO, GYRO_DI, GYRO_CLK);
#endif
#define SAMPLES 100

long int t;
int elapsedtime;
float err_x;
float err_y;
float err_z;

float deg_x;
float deg_y;
float deg_z;
int i;
bool SkiGyroConnected;
char outstr[50];

void setup()
{
  Serial.begin(9600);
//  RFduinoBLE.advertisementData = "temp";
  RFduinoBLE.deviceName = "SkiGyro";

  // start the BLE stack
  RFduinoBLE.begin();

  // Try to initialise and warn if we couldn't detect the chip
  if (!gyro.begin(gyro.L3DS20_RANGE_250DPS))
    //if (!gyro.begin(gyro.L3DS20_RANGE_500DPS))
    //if (!gyro.begin(gyro.L3DS20_RANGE_2000DPS))
  {
    Serial.println("Oops ... unable to initialize the L3GD20. Check your wiring!");
    while (1);
  };
  err_x = 0;
  err_y = 0;
  err_z = 0;
  deg_x = 0;
  deg_y = 0;
  deg_z = 0;
  for (i = 0; i < SAMPLES ; i++)
  {
    gyro.read();
    err_x += gyro.data.x;
    err_y += gyro.data.y;
    err_z += gyro.data.z;
    delay(10);
  };
  err_x /= SAMPLES;
  err_y /= SAMPLES;
  err_z /= SAMPLES;
  i = 0;
  t = millis();
};

void loop()
{
  while (SkiGyroConnected == true)
  {
    elapsedtime = millis() - t;
    t = millis();
    gyro.read();
    deg_x += (gyro.data.x - err_x) * elapsedtime;
    deg_y += (gyro.data.y - err_y) * elapsedtime;
    deg_z += (gyro.data.z - err_z) * elapsedtime;
    //   delay(10);
    if (++i >= 1000)
    {
      if (abs(deg_x) < 10000.0) deg_x = 0;
      if (abs(deg_y) < 10000.0) deg_y = 0;
      if (abs(deg_z) < 10000.0) deg_z = 0;
      RFduinoBLE.sendInt(0xabadcafe);
      RFduinoBLE.sendFloat(deg_x / 1000);
      RFduinoBLE.sendFloat(deg_y / 1000);
      RFduinoBLE.sendFloat(deg_z / 1000);
      sprintf(outstr,"X=%f",deg_x/1000);
      if (deg_x != 0.0) {
        sprintf(outstr,"X= %d",(int)deg_x/1000);
        Serial.println(outstr);
        RFduinoBLE.send(outstr,strlen(outstr));
      }
      else if (deg_y != 0.0) {
        sprintf(outstr,"Y= %d",(int)deg_y/1000);
        Serial.println(outstr);
        RFduinoBLE.send(outstr,strlen(outstr));
      }
      else if (deg_z != 0.0) {
        sprintf(outstr,"Z= %d",(int)deg_z/1000);
        Serial.println(outstr);
        RFduinoBLE.send(outstr,strlen(outstr));
     };
      deg_x = 0;
      deg_y = 0;
      deg_z = 0;
      i = 0;
    };
  };
    // switch to lower power mode
  RFduino_ULPDelay(SECONDS(10));  
}

void RFduinoBLE_onConnect()
{
  SkiGyroConnected = true;
  Serial.println("Connected");
}

void RFduinoBLE_onDisconnect()
{
  SkiGyroConnected = false;
  Serial.println("Disconnected");
}
