/*
  Created by: Bradley Henderson
  MAE: 490 Liftoff team 3 IMU Data Saving test
  Last Updated: 7/8/2017
 */
#include <math.h>
#include <SD.h>
#include "MPU9250.h"
File dataFile;
MPU9250 myIMU;
// Pin definitions
const int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
const int chipSelect = 8;  //Chip Select pin is tied to pin 8 on the SparkFun SD Card Shield
//
//String filename = "IMUdataSavingTest1.txt";   //Filename for the data
int i = 0;
const float AREF = 3.3;
void setup() {

/////// IMU SETUP //////
Wire.begin();
// TWBR = 12;  // 400 kbit/sec I2C speed
Serial.begin(38400);

// Set up the interrupt pin, its set as active high, push-pull
pinMode(intPin, INPUT);
digitalWrite(intPin, LOW);
pinMode(10, OUTPUT);


// Calibrate gyro and accelerometers, load biases in bias registers
myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
myIMU.initMPU9250();
// Get magnetometer calibration from AK8963 ROM
myIMU.initAK8963(myIMU.factoryMagCalibration);

// Get sensor resolutions, only need to do this once
myIMU.getAres();
myIMU.getGres();
myIMU.getMres();

// The next call delays for 4 seconds, and then records about 15 seconds of
// data to calculate bias and scale.
myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);


//Check on SD card//
Serial.print("Initializing SD card...");
// make sure that the default chip select pin is set to
// output, even if you don't use it:
pinMode(chipSelect, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}
void loop() {
 
 Serial.println("in main loop");
 Serial.println(i);
 i++;

   //// IMU Section ////
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    // Calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = 1000*(((float)myIMU.accelCount[0] * myIMU.aRes) - myIMU.accelBias[0]);
    myIMU.ay = 1000*(((float)myIMU.accelCount[1] * myIMU.aRes) - myIMU.accelBias[1]);
    myIMU.az = 1000*(((float)myIMU.accelCount[2] * myIMU.aRes) - myIMU.accelBias[2]);
    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;
    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
   Serial.println("After IMU stuff");
    String dataString = "";
    dataString += String(millis());
    dataString += ",";
    dataString += String(myIMU.roll);
    dataString += ",";
    dataString += String(myIMU.pitch);
    dataString += ",";
    dataString += String(myIMU.yaw);
    dataString += ",";
    dataString += String(myIMU.ax);
    dataString += ",";
    dataString += String(myIMU.ay);
    dataString += ",";
    dataString += String(myIMU.az);
    //dataString += ",";
    //dataString += String(currentAltitudeA);
    //dataString += ",";
    //dataString += String(currentAltitudeB);
    //dataString += ",";
    //dataString += String(pressureInternal());
    //dataString += ",";
    //dataString += String(internalTemp());
    Serial.println(dataString);
    //saveData(dataString,filename);
  File dataFile = SD.open("sh.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("error opening datalog.txt");
  } 
    delay(500);
}  //void loop


unsigned int pressAltitude(unsigned long pressure){   //good until about 65,000 ft.
  unsigned long pressureMillibar = pressure/100;                                      //converts Pa to mBar
  unsigned int  pressAltitude = (1-(pow((pressureMillibar/1013.25),0.190284)))*145366.45;    //calculates altitude in FT
  return pressAltitude;
}

void servoDeploy(){
  ///// put code here to deploy the parachute
  ////
}

