//******************************************************************
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <Keyboard.h>
//#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary
Adafruit_MMA8451 mma = Adafruit_MMA8451();
//******************************************************************

/* settings */

long avgX;
const int numReadings = 128;  // multiples of 2^2
const int volume = 1;        // 128 will be loudest, go down to about 10. 0=Off. 
int readings[numReadings];    // the readings from the analog input
int readIndex = 0;            // the index of the current reading
uint32_t total = 0;           // the running total
int average,average2 = 0;
boolean swOn = false;
int sensorValue;
long tick;
long tout;
int onTmr;
boolean ttmr = false;
char keyCode = KEY_LEFT_SHIFT;      //For the correct code see https://github.com/arduino-libraries/Keyboard/blob/a7abf94998c54f01e879d6c0e3fac3169b4a76c2/src/Keyboard.h#L37
int counter; 
boolean debug = false;

/* Main */

/*
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  2, 0,                  // Button Count, Hat Switch Count
  false, false, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering
*/

void setup() {

/*
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
*/  
  Keyboard.begin();
  pinMode(10,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT); 
  digitalWrite(4,HIGH);   
  //TCCR1B = TCCR1B & B11111000 | B00000011;  // 64 for PWM  490.20 Hz (The DEFAULT)
  TCCR1B = TCCR1B & B11111000 | B00000010;  // 1/8 for PWM frequency of  3921.16 Hz
  delay(5000);     
  Serial.begin(115200);
  Serial.println("Adafruit MMA8451 test!");
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  //Startup sound
  for(counter = 2;counter <= 50;counter++) {
    analogWrite(10,counter); delay(20); analogWrite(10,0);  
  } 
}

void loop() {
  // put your main code here, to run repeatedly:
  mma.read();
  total = total - readings[readIndex]; // subtract the last reading:
  readings[readIndex] = mma.x; // read from the sensor:
  total = total + readings[readIndex];  // add the reading to the total:
  readIndex = readIndex + 1; // advance to the next position in the array:
  if (readIndex >= numReadings){ 
   readIndex = 0;   //if we're at the end of the array wrap around
  }
  average = total / numReadings; //lock average if switch is on
  onTmr++;

    if (mma.x < average-100) //switch triggered if < 100 from average
    {

   if (debug){
     Serial.print("avg:");
     Serial.print(average);
     Serial.print("    :");
     Serial.println(mma.x);
   }  
      delay(20);
      mma.read();       
      if (onTmr <  25)
      {
          tick=millis(); //say 30,000 in tick                  
          analogWrite(10,volume);   // sound
          digitalWrite(2,LOW);
          Keyboard.print(keyCode);
          //Joystick.setButton(0, 1);
          if (debug){ Serial.println("Press"); }       
      }
      else
      {
          analogWrite(10,0);
          digitalWrite(2,HIGH);
          //Joystick.setButton(0, 0); 
      }              
    }
      else
      {
        if ((millis()-tick )> 250) //must be > 50,000
        {
          analogWrite(10,0);
          digitalWrite(2,HIGH);
          //Joystick.setButton(0, 0);
        }
         onTmr = 0;       
}
delay(20);
}
