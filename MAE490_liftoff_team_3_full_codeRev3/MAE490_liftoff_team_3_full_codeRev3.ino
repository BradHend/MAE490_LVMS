/*
  Created by: Bradley Henderson
  MAE: 490 Liftoff team 3 Flight Software
  Last Updated: 5/29/2017
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
String filename = "flightdatatest3.txt";   //Filename for the data

const float alpha      = 0.6;     //exponential smoothing factor
const float Vs_pressA  = 3.3;     //Vs for pressure sensor A
const float Vs_pressB  = 5.0;     //Vs for pressure sensor B
const float Vs_therm   = 3.3;     //Vs for thermistor A

const float AREF          = 3.3;     //Reference voltage for the ADC
const int pressureA_pin   = 0;
const int pressureB_pin   = 1;
const int internalPressurePin = 2;
const int thermistorPin   = 3;
const unsigned int R1_pressInternal = 10000;       //R1 value for internal pressure
//const float PressIncr = 3.0;                //times to read ADC for pressure calc. (also used for temperature)

///thermistor coefficients and resistances///
const int Rref = 10000;
const int R1_therm = 10000;        //R1 value for voltage divider at thermistor
float a1 = 0.003354016;
const float b1 = 0.0002569850;
const float c1 = 0.000002620131;
const float D1 = 0.00000006383091;

///creating global variables///
unsigned long pressure_A = 0;
unsigned long pressure_B = 0;
int currentAltitudeA  = 0;
int currentAltitudeB  = 0;
int previousAltitudeA = 0;
int previousAltitudeB = 0;
int initialAltitudeA  = 0;
int initialAltitudeB  = 0;
int prevAltitudesA[9];
int prevAltitudesB[9];
unsigned int i = 0;
bool parachuteA = false;
bool parachuteB = false;
bool startUp = true;

void setup() {
pinMode(pressureA_pin, INPUT);
pinMode(pressureB_pin, INPUT);
pinMode(internalPressurePin, INPUT);
pinMode(thermistorPin, INPUT);
analogReference(EXTERNAL);

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
//myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);


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


// code to read ADC a few times to "throw away" initial bad measurments
int junk = 0;
  for(int j; j<50; j++){                  //loops a number of times to get average
    junk += analogRead(pressureA_pin);    //read pin for pressure sensor A's output
    junk += analogRead(pressureB_pin);    //read pin for pressure sensor B's output
    junk += analogRead(internalPressurePin);    //read pin for internal pressure sensor output
    junk += analogRead(thermistorPin);    //read pin for thermistor output    
  }
  junk = 0;                  //zero out
}

void loop() {

if (startUp == true){
  // find the initial pressure altitude
   for(int ii; ii<15; ii++){           //loops a number of times to get average
    pressure_A = pressure(pressureA_pin);
    Serial.println(pressure_A);
    pressure_B = pressure(pressureB_pin);
    Serial.println(pressure_B);
    initialAltitudeA += pressAltitude(pressure_A); //altitude for pressure A
    initialAltitudeB += pressAltitude(pressure_B); //altitude for pressure B
    Serial.println(initialAltitudeA);
    Serial.println(initialAltitudeB);
    delay(50);
  }
initialAltitudeA  = initialAltitudeA/15;                 //average, used for initial altitude
previousAltitudeA = initialAltitudeA;                      //sets variable to intitial alt
currentAltitudeA  = initialAltitudeA;                      //sets variable to intitial alt 
initialAltitudeB  = initialAltitudeB/15;                 //average, used for initial altitude
previousAltitudeA = initialAltitudeB;                      //sets variable to intitial alt
currentAltitudeB  = initialAltitudeB;                      //sets variable to intitial alt 
Serial.print("iniitialAltA is: ");
Serial.print(initialAltitudeA);
Serial.print(" ");
Serial.print("iniitialAltB is: ");
Serial.println(initialAltitudeB);
startUp = false;
delay(5000);
}
 
 Serial.println("in main loop");
 Serial.println(i);
 i++;

   //// Altitude Section ////

  pressure_A = pressure(pressureA_pin);
  currentAltitudeA = (((1-alpha)*pressAltitude(pressure_A)) + (alpha*previousAltitudeA)) - initialAltitudeA;
  pressure_B = pressure(pressureB_pin);
  currentAltitudeB = (((1-alpha)*pressAltitude(pressure_B)) + (alpha*previousAltitudeB)) - initialAltitudeB;
 Serial.print("pressure A is:");
 Serial.println(pressure_A);
 Serial.print("pressure B is:");
 Serial.println(pressure_B);
 Serial.print("Current Alt. A is:");
 Serial.println(currentAltitudeA);
 Serial.print("Current Alt. B is:");
 Serial.println(currentAltitudeB);
 if (i>9){
//  if ((currentAltitudeA>30) & (currentAltitudeB>30)){
//      if ((parachuteA=true)&(parachuteB=true)){
//      servoDeploy();  // deploy the parachute
//       }
//      else{
//        unsigned long set1A = prevAltitudesA[0] + prevAltitudesA[1] + prevAltitudesA[2] + prevAltitudesA[3];
//       unsigned long set2A = prevAltitudesA[6] + prevAltitudesA[7] + prevAltitudesA[8] + prevAltitudesA[9];
//        unsigned long set1B = prevAltitudesB[0] + prevAltitudesB[1] + prevAltitudesB[2] + prevAltitudesB[3];
//        unsigned long set2B = prevAltitudesB[6] + prevAltitudesB[7] + prevAltitudesB[8] + prevAltitudesB[9];
//        if ((set1A - set2A)>0){ 
//           parachuteA = true;
//           }
//        if ((set1B - set2B)>0){ 
//            parachuteB = true;
//           }
//        }
//  }
  i = 0;
 }

 prevAltitudesA[i] = currentAltitudeA;
 prevAltitudesB[i] = currentAltitudeB;
 
   //// IMU Section ////
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    // Calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes; // - myIMU.accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes; // - myIMU.accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes; // - myIMU.accelBias[2];
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
    dataString += ",";
    dataString += String(currentAltitudeA);
    dataString += ",";
    dataString += String(currentAltitudeB);
    dataString += ",";
    dataString += String(pressureInternal());
    dataString += ",";
    dataString += String(internalTemp());
    Serial.println(dataString);
    delay(500);
}  //void loop

unsigned long pressure(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  unsigned int vOut = 0;
  float Output = 0.0;
//const float  Vs_pressA = 3.3;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    //Serial.println(vOut);
    delay(1);
    }
  //delay(10);
  Output = (vOut/9.0)*(AREF/1024);                  //average voltage reading to be used in pressure calc.
  Serial.println(Output);
  float pressure = 1000*(((Output/3.3)+0.095)/0.009);  //pressure calc.
  return (unsigned long) pressure;
 }

float pressureInternal(){                        //finds the pressure value given by pressure sensor A
  float pressureIntern = 0;
  float vOutPressInt = 0;
  for(int i; i<10; i++){           //loops a number of times to get average
    vOutPressInt += analogRead(pressureB_pin);    //read pin for pressure sensor A's output
    delay(1);
  }
  vOutPressInt = ((vOutPressInt/10.0)*(AREF/1024))*2.0;              //accounting for voltage divider
  Serial.println(vOutPressInt);
  pressureIntern = (vOutPressInt-0.5)*37.5;
  return pressureIntern;
}

unsigned int pressAltitude(unsigned long pressure){   //good until about 65,000 ft.
  unsigned long pressureMillibar = pressure/100;                                      //converts Pa to mBar
  unsigned int  pressAltitude = (1-(pow((pressureMillibar/1013.25),0.190284)))*145366.45;    //calculates altitude in FT
  return pressAltitude;
}

////////// Needs to be fixed to allow it to run faster//////////
float internalTemp(){                        //finds the internal temperature of the bottle using the thermistor
  unsigned long tempCentiK = 0;
  unsigned long tempCentiC = 0;
  float vOutTherm = 0;
  for(int i; i<10; i++){                //loops a number of times to get average
    vOutTherm += analogRead(thermistorPin);    //read pin for thermistor's output
  }
  vOutTherm = (vOutTherm/10.0)*(AREF/1024);                  //average voltage reading to be used in temp calc.
  float R = R1_therm/((Vs_therm/vOutTherm)-1);
  tempCentiK = 100*(1.0/(a1 + (b1*log(R/Rref))+ (c1*pow((log(R/Rref)),2)) + (D1*pow((log(R/Rref)),3))));  //temp calc in CentiK
  tempCentiC = tempCentiK - 27315;       //convert temp in K to C (easier for students to know how Celsius temps feel?)
  return tempCentiC;
}

void servoDeploy(){
  ///// put code here to deploy the parachute
  ////
}

//////////SD Card Data Saving function/////////
void saveData(String dataString){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // this opens the file and appends to the end of file
  // if the file does not exist, this will create a new file.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile)   {  
    //write to uSD card
    dataFile.print(dataString);
  }  
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("error opening datalog.txt");
  } 
}

