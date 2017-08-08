/*
  Created by: Bradley Henderson
  MAE: 490 Liftoff team 3 Flight Software
  Last Updated: 8/7/2017
 */
#include <math.h>

bool startUp = true;
const float alpha      = 0.6;     //exponential smoothing factor
//const float Vs_pressA  = 3.3;     //Vs for pressure sensor A
const float Vs_pressB  = 5.0;     //Vs for pressure sensor B
const float Vs_therm   = 3.3;     //Vs for thermistor A

const float AREF          = 3.3;     //Reference voltage for the ADC
const int pressureA_pin   =  0;
const int pressureB_pin   =  1;
unsigned long pressure_A = 0;
unsigned long pressure_B = 0;
const float PressIncr = 3.0;                //times to read ADC for pressure calc. (also used for temperature)


///creating global variables///
unsigned long currentPressureA = 0;
unsigned long currentPressureB = 0;
float currentAltitudeA = 0;
float currentAltitudeB = 0;
float previousAltitudeA = 0;
float previousAltitudeB = 0;
float initialAltitudeA = 0;
float initialAltitudeB = 0;



void setup() {

pinMode(pressureA_pin, INPUT);
pinMode(pressureB_pin, INPUT);
analogReference(EXTERNAL);
Serial.begin(9600);


// code to read ADC a few times to "throw away" initial bad measurments
int junk = 0;
  for(int i; i<100; i++){                 //loops a number of times to get average
    junk += analogRead(pressureA_pin);    //read pin for pressure sensor A's output
    delay(10);
    junk += analogRead(pressureB_pin);    //read pin for pressure sensor B's output
    delay(10);
  }
  junk = 0;                  //zero out
}



void loop() {

if (startUp == true){
  // find the initial pressure altitude
   for(int i; i<10; i++){           //loops a number of times to get average
    currentPressureA = pressure(pressureA_pin);
    Serial.println(currentPressureA);
    currentPressureB = pressure(pressureB_pin);
    Serial.println(currentPressureB);
    initialAltitudeA += pressAltitude(currentPressureA); //altitude for pressure A
    initialAltitudeB += pressAltitude(currentPressureB); //altitude for pressure B
    Serial.println(initialAltitudeA);
    Serial.println(initialAltitudeB);
    delay(1);
  }
initialAltitudeA  = initialAltitudeA/10.0;                 //average, used for initial altitude
previousAltitudeA = initialAltitudeA;                      //sets variable to intitial alt
currentAltitudeA  = initialAltitudeA;                      //sets variable to intitial alt 
initialAltitudeB  = initialAltitudeB/10.0;                 //average, used for initial altitude
previousAltitudeA = initialAltitudeB;                      //sets variable to intitial alt
currentAltitudeB  = initialAltitudeB;                      //sets variable to intitial alt 
  Serial.print("iniitialAltA is: ");
  Serial.print(initialAltitudeA);
  Serial.print(" ");
  Serial.print("iniitialAltB is: ");
  Serial.println(initialAltitudeB);
delay(1000);
startUp = false;
}


  pressure_A = pressure(pressureA_pin);
    //currentAlt = pressAltitudeA(pressure)
//    Serial.print("Time is: ",millis()/1000.0);
  Serial.print("PressureA is: ");
  Serial.print(pressure_A);
  Serial.print(" ");
  Serial.print("Alt_A is: ");
  currentAltitudeA = pressAltitude(pressure_A)-initialAltitudeA;
  Serial.println(currentAltitudeA,4);
  delay(100);
  pressure_B = pressure(pressureB_pin);
  Serial.print("PressureB is: ");
  Serial.print(pressure_B);
  Serial.print(" ");
  Serial.print("Alt_B is: ");
  currentAltitudeB = pressAltitude(pressure_B)-initialAltitudeB;
  Serial.println(currentAltitudeB,4);
  delay(100);
    
    //Serial.println("Altitude is: ",currentAlt);
}


unsigned long pressure(int pin){                        //finds the pressure value given by pressure sensor on specified pin
  unsigned int vOut = 0;
//const float  Vs_pressA = 3.3;
  for(int i; i<9; i++){           //loops a number of times to get average
    vOut += analogRead(pin);    //read pin for pressure sensor A's output
    //Serial.println(vOut);
    delay(2);
    }
  //delay(10);
  float Output = (vOut/9.0)*(AREF/1023.0);                  //average voltage reading to be used in pressure calc.
  Serial.println(Output, 3);
  float pressure_val = 1000*(((Output/3.3)+0.095)/0.009);  //pressure calc.
  return (unsigned long) pressure_val;
 }
 
unsigned int pressAltitude(unsigned long pressure){
  float pressureMillibar = pressure/100;                                      //converts kPa to mBar
  float  pressAlt = (1-(pow((pressureMillibar/1013.25),0.190284)))*145366.45;    //calculates altitude in FT
  Serial.println(pressAlt,2);
  return (unsigned int) pressAlt;
}




