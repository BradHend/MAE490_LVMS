/*
 Liftoff Team #3
 Revised-revised flight software, with servo
 Created by: Bradley Henderson
 Date: 11/4/2017
 */

#include <math.h>
#include "MPU9250.h"
//#include "quaternionFilters.h"
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
MPU9250 myIMU;
Servo myservo;

// The following are variables to be changed by user ///
String    FILENAME = "fligh3.txt"; 
const int deployTime = 500; // 1 second deploy time
const int stopTime = 9000; // 3 second LED stop time
const int servoPin = 9;
float gTrigger = -2.5;
const int servoClose = 0; // servo close angle
const int servoOpen = 80; // servo open angle
////////////////////////////

///creating global variables///
const int chipSelect      = 8; // Chip Select pin is tied to pin 8 on the SparkFun SD Card Shield
const int intPin          = 12; 
const int pressureA_pin   =  1;
const int pressureB_pin   =  2;
const int intPressurePin  = 0;
const int thermistorPin   = 3;
int pressure_A  = 0;
int pressure_B  = 0;
unsigned long timeStamp   = 0;
unsigned long state1Time  = 0;

float intPress = 0;
unsigned int intTempV = 0;

bool startUp = true;
byte flightState = 0;


void setup()
{
  // Open serial communications and wait for port to open:
Serial.begin(250000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

analogReference(EXTERNAL);      // sets referece to external for ADC, (connected to 3.3V)

/////// IMU SETUP //////
Wire.begin();
// TWBR = 12;  // 400 kbit/sec I2C speed
// Set up the interrupt pin, its set as active high, push-pull
pinMode(intPin, INPUT);
digitalWrite(intPin, LOW);
pinMode(10, OUTPUT);

myservo.attach(servoPin); // attach servo to pin servoPin
myservo.write(servoClose); // closes servo

// Calibrate gyro and accelerometers, load biases in bias registers
//myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
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

void loop()
{
 pressure_A = pressureA(pressureA_pin);
 pressure_B = pressureB(pressureB_pin);
 intPress = pressureInternal();
 intTempV = intTemperature(thermistorPin);

switch (flightState){
    case 0:
      {
        myservo.write(servoClose); // closes servo
        flightState = 1;
      } 
      break;
      
    case 1:
      {
        if (myIMU.ax <= (gTrigger * 1000))
        {
          state1Time = timeStamp;
          flightState = 2;
        }
       else
        {
          flightState = 1; 
          myservo.write(servoClose); // closes servo
        }
      }
      break;
      
    case 2:
        {
          if ((timeStamp - state1Time) > deployTime) {
            servoDeploy();
            myservo.write(servoOpen); // open servo
          }
          else
          {
            flightState = 2;
          }
        }
      break;
    }


 Serial.print("FlightState: ");
 Serial.println(flightState);
  
// If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual milli-g's
    // This depends on scale being set
    myIMU.ax = 1000*(((float)myIMU.accelCount[0] * myIMU.aRes));// + 1128;
    myIMU.ay = 1000*(((float)myIMU.accelCount[1] * myIMU.aRes));// + 125;
    myIMU.az = 1000*(((float)myIMU.accelCount[2] * myIMU.aRes));// + 947;

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


  if (flightState != 0 )
  {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // this opens the file and appends to the end of file
  // if the file does not exist, this will create a new file.
 File dataFile = SD.open(FILENAME, FILE_WRITE);
 
  // if the file is available, write to it:
  if (dataFile)   {  
    timeStamp = millis();
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
    dataFile.print(intPress);
    Serial.print(intPress);
    Serial.print(",");
    dataFile.print(",");
    dataFile.print(intTempV);
    Serial.print(intTempV);

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

  }

}


int pressureA(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  int vOut = 0;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  return vOut;
 }

int pressureB(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  int vOut = 0;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  return vOut;
 }

float pressureInternal(){                        //finds the pressure value given by pressure sensor A
  float vOutPressInt = 0;
  for(int i; i<3; i++){           //loops a number of times to get average
    vOutPressInt += analogRead(intPressurePin);    //read pin for pressure sensor A's output
    delay(1);
  }
  vOutPressInt = ((vOutPressInt/3.0)*(3.3/1023.0))*2.0;              //accounting for voltage divider
  float intPressure = 100*((vOutPressInt-0.5)*37.5);
  return intPressure;
}

unsigned int intTemperature(int pin){
  unsigned int vOut = 0;
  for(int i; i<3; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    delay(1);
    }
  float Output = (vOut/3.0)*(3.3/1.0230);                  //average voltage reading to be used in pressure calc.
  return (unsigned int) Output;
}

void servoDeploy(){
  ///// put code here to deploy the parachute
     myservo.write(servoOpen);
}
 
