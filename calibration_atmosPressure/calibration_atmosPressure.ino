/*
 Liftoff Team #3
 Atmospheric Pressure sensor calibration program, with saving to SD card
 Created by: Bradley Henderson
 Date: 8/15/2017
 */


#include <SD.h>
#include <SPI.h>

// Chip Select pin is tied to pin 8 on the SparkFun SD Card Shield
const int chipSelect = 8;  
const int pressureA_pin   =  0;
const int pressureB_pin   =  1;
unsigned long pressure_A = 0;
unsigned long pressure_B = 0;

///creating global variables///
unsigned long currentPressureA = 0;
unsigned long currentPressureB = 0;
float currentAltitudeA = 0;
float currentAltitudeB = 0;



void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

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
    delay(10);
    junk += analogRead(pressureB_pin);    //read pin for pressure sensor B's output
    Serial.println(junk);
    delay(10);   
  }
  junk = 0;                  //zero out
}

void loop()
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // this opens the file and appends to the end of file
  // if the file does not exist, this will create a new file.
 File dataFile = SD.open("pAtCal1.txt", FILE_WRITE);
 pressure_A = pressureA(pressureA_pin);
 pressure_B = pressureB(pressureB_pin);
 currentAltitudeA = pressAltitude(pressure_A);
 currentAltitudeB = pressAltitude(pressure_B);
 
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
    dataFile.println(currentAltitudeB);
    Serial.println(currentAltitudeB);
    //Serial.print(",");
    //dataFile.print(",");
    dataFile.close();   //close file

  }  
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("error opening datalog.txt");
  } 
}


unsigned long pressureA(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  unsigned int vOut = 0;
//const float  Vs_pressA = 3.3;
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
//const float  Vs_pressA = 3.3;
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



 
