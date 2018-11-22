long avgX;
const int numReadings = 128;  //multiples of 2^2
const int volume = 2; //128 will be loudest, go down to about 10

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
uint32_t total = 0;                  // the running total
int average,average2 = 0;
boolean swOn = false;
int sensorValue;
long tick;
long tout;
int onTmr;
char debounce;
char keyCode = 'p';
boolean ttmr = false;
//******************************************************************
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <Keyboard.h>
Adafruit_MMA8451 mma = Adafruit_MMA8451();
//******************************************************************



void setup() {

Keyboard.begin();

   pinMode(10,OUTPUT);
   pinMode(2,OUTPUT);
   pinMode(4,OUTPUT); 
   digitalWrite(4,HIGH);   
 //TCCR1B = TCCR1B & B11111000 | B00000011;  // 64 for PWM  490.20 Hz (The DEFAULT)
   TCCR1B = TCCR1B & B11111000 | B00000010;  // 1/8 for PWM frequency of  3921.16 Hz

       analogWrite(10,volume); 
      delay(100);
       analogWrite(10,0); 
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
  Serial.println("G");
   
   
   
   
   

}

void loop() {
  // put your main code here, to run repeatedly:
  mma.read();
  total = total - readings[readIndex]; // subtract the last reading:
  
  //sensorValue = analogRead(A0);
  //readings[readIndex] = sensorValue; // read from the sensor:
  readings[readIndex] = mma.x; // read from the sensor:
  total = total + readings[readIndex];  // add the reading to the total:
  readIndex = readIndex + 1; // advance to the next position in the array:
  if (readIndex >= numReadings) 
   readIndex = 0;   //if we're at the end of the array wrap around
   average = total / numReadings; //lock average if switch is on
      onTmr++;
 //   Serial.print(average);
 //   Serial.print("    :");
 //   Serial.println(mma.x);
    
    if (mma.x < average-80) //switch triggered if < 100 from average
    {
    delay(20);
       mma.read();
       
      if (mma.x< average-80)

            {
              if (onTmr <  25)
              {
                  tick=millis(); //say 30,000 in tick 

                 
              analogWrite(10,volume);   // sound
              digitalWrite(2,LOW);
              Keyboard.print("p");
              //Joystick.setButton(0, 1);              
              }
              else
              {
                      analogWrite(10,0);
              digitalWrite(2,HIGH);
              //Joystick.setButton(0, 0);
              
              }
              
           
              
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