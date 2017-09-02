/*
 Liftoff Team #3
 Full sensor suite testing, with saving to SD card
 Created by: Bradley Henderson
 Date: 8/16/2017
 */

#include <math.h>
#include "MPU9250.h"
#include "quaternionFilters.h"
#include <SD.h>
#include <SPI.h>
MPU9250 myIMU;


const int chipSelect = 8; // Chip Select pin is tied to pin 8 on the SparkFun SD Card Shield
const int intPin = 12;    // These can be changed, 2 and 3 are the Arduinos ext int pins  
const int pressureA_pin   =  0;
const int pressureB_pin   =  1;
const int intPressurePin = 2;
const int thermistorPin   = 3;
unsigned long pressure_A = 0;
unsigned long pressure_B = 0;

///creating global variables///
unsigned long currentPressureA = 0;
unsigned long currentPressureB = 0;
float currentAltitudeA  = 0;
float currentAltitudeB  = 0;
float previousAltitudeA = 0;
float previousAltitudeB = 0;
float initialAltitudeA = 0;
float initialAltitudeB = 0;

float intPress = 0;
unsigned int intTempV = 0;

bool startUp = true;
byte flightState = 0;

void setup()
{
  // Open serial communications and wait for port to open:
Serial.begin(38400);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

/////// IMU SETUP //////
Wire.begin();
// TWBR = 12;  // 400 kbit/sec I2C speed


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
//myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);

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

  // code to read ADC a few times to "throw away" initial bad measurments
int junk = 0;
  for(int j; j<100; j++){                  //loops a number of times to get average
    junk += analogRead(pressureA_pin);    //read pin for pressure sensor A's output
    Serial.println(junk);
    junk += analogRead(pressureB_pin);    //read pin for pressure sensor B's output
    Serial.println(junk);
    junk += analogRead(intPressurePin);    //read pin for pressure sensor B's output
    Serial.println(junk);
    junk += analogRead(thermistorPin);    //read pin for pressure sensor B's output
    Serial.println(junk);
    delay(10);   
  }
  junk = 0;                  //zero out
}

void loop(){

// If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01){
    
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual milli-g's
    // This depends on scale being set
    myIMU.ax = 1000*((float)myIMU.accelCount[0] * myIMU.aRes); // - myIMU.accelBias[0];
    myIMU.ay = 1000*((float)myIMU.accelCount[1] * myIMU.aRes); // - myIMU.accelBias[1];
    myIMU.az = 1000*((float)myIMU.accelCount[2] * myIMU.aRes); // - myIMU.accelBias[2];

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  myIMU.updateTime();
  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);
      myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ()
                    * *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ()
                    * *(getQ()+2)));
      myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2)
                    * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw   *= RAD_TO_DEG;
      myIMU.yaw  += 3.683;
      myIMU.roll *= RAD_TO_DEG;

  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // this opens the file and appends to the end of file
  // if the file does not exist, this will create a new file.
 File dataFile = SD.open("senTest7.txt", FILE_WRITE);
 pressure_A = pressureA(pressureA_pin);
 pressure_B = pressureB(pressureB_pin);
 currentAltitudeA = pressAltitude(pressure_A);
 currentAltitudeB = pressAltitude(pressure_B);
  if (startUp == true){
  delay(1000);
  initialAltitudeA = currentAltitudeA;
  previousAltitudeA = currentAltitudeA;
  initialAltitudeB = currentAltitudeB;
  previousAltitudeB = currentAltitudeB;
  startUp = false;
  }
 currentAltitudeA = ((0.6*currentAltitudeA)+(0.4*previousAltitudeA)) - initialAltitudeA;
 currentAltitudeB = ((0.6*currentAltitudeB)+(0.4*previousAltitudeB)) - initialAltitudeB;

 previousAltitudeA = currentAltitudeA;
 previousAltitudeB = currentAltitudeB;
 
 intPress = pressureInternal();
 intTempV = intTemperature(thermistorPin);
 
  // if the file is available, write to it:
  if (dataFile)   {  
    unsigned long timeStamp = millis();
    //write to uSD card
    dataFile.print(timeStamp);
    Serial.print(timeStamp);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(pressure_A);
    Serial.print(pressure_A);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(pressure_B);
    Serial.print(pressure_B);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(currentAltitudeA);
    Serial.print(currentAltitudeA);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(currentAltitudeB);
    Serial.print(currentAltitudeB);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(intPress);
    Serial.print(intPress);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(intTempV);
    Serial.print(intTempV);
    
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.yaw);
    Serial.print(myIMU.yaw,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.pitch);
    Serial.print(myIMU.pitch,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.roll);
    Serial.print(myIMU.roll,2);

    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.ax);
    Serial.print(myIMU.ax,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.ay);
    Serial.print(myIMU.ay,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.az);
    Serial.print(myIMU.az,2);

    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.mx);
    Serial.print(myIMU.mx,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.my);
    Serial.print(myIMU.my,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.mz);
    Serial.print(myIMU.mz,2);

    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.gx);
    Serial.print(myIMU.gx,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(myIMU.gy);
    Serial.print(myIMU.gy,2);
    Serial.print(",");
    dataFile.print(",");
    dataFile.println(myIMU.gz);
    Serial.println(myIMU.gz,2);

    dataFile.close();   //close file

  }  
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("error opening datalog.txt");
  } 



 if (flightState == 0){
    //check if altitude is > 100ft, if so start counting
    if (currentAltitudeB > (initialAltitudeB + 100)){
      fligthState = 1;
      unsigned long state1Time = timeStamp;
    }
    else{
      flightState = 0; 
    }
 }
 
 if (flightState == 1){
   if ((timeStamp - state1Time) > deployTime) {
        servoDeploy();
    }
 }
 
}


unsigned long pressureA(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  unsigned int vOut = 0;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  float Output = (vOut/9.0)*(3.3/1023.0);                  //average voltage reading to be used in pressure calc.
  Serial.println(Output, 4);
  float pressure_valA = 1000*(((Output/3.3)+0.095)/0.009);  //pressure calc.
  return (unsigned long) pressure_valA;
 }

unsigned long pressureB(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  unsigned int vOut = 0;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  float Output = (vOut/9.0)*(3.3/1023.0);                  //average voltage reading to be used in pressure calc.
  Serial.println(Output, 4);
  float pressure_valB = 1000*(((Output/3.3)+0.095)/0.009);  //pressure calc.
  return (unsigned long) pressure_valB;
 }

unsigned int pressAltitude(unsigned long pressure){
  float pressureMillibar = pressure/100.0;                                      //converts kPa to mBar
  float  pressAlt = 10*((1-(pow((pressureMillibar/1013.25),0.190284)))*145366.45);    //calculates altitude in FT
  Serial.println(pressAlt,2);
  return (unsigned int) pressAlt;
}

float pressureInternal(){                        //finds the pressure value given by pressure sensor A
  float intPressure= 0;
  float vOutPressInt = 0;
  for(int i; i<3; i++){           //loops a number of times to get average
    vOutPressInt += analogRead(intPressurePin);    //read pin for pressure sensor A's output
    delay(1);
  }
  vOutPressInt = ((vOutPressInt/3.0)*(3.3/1023.0))*2.0;              //accounting for voltage divider
  Serial.println(vOutPressInt);
  intPressure = (vOutPressInt-0.5)*37.5;
  return intPressure;
}

unsigned int intTemperature(int pin){
  unsigned int vOut = 0;
  for(int i; i<3; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  float Output = (vOut/3.0)*(3.3/1.0230);                  //average voltage reading to be used in pressure calc.
  Serial.println(Output, 4);
  return (unsigned int) Output;
}

void servoDeploy(){
  ///// put code here to deploy the parachute
  ////
}
 
