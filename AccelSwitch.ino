/* AccelerometerSwitch Code
 * Version: 2.0
 * Authors: Will Wade (Ace Centre), Gerard Cullen (RHN Putney), Paul Hewett (Ace Centre)
 * 
 * Whats it do?: Add a MMA8451 (Connect to SCL/SDA pins) and a buzzer (by default: Pin10) on a arduino
 *                and this presses a key on the keyboard when the acceleration of one or more axis is above a % difference
 *                
 * Changes over v1: v1 had a simple circular buffer. It was ok but a couple of problems existed. It could be too slow to record a change due to the size of the 
 *                    buffer (average would take a while to change) - and the threshold setting was a tad unreliable. 
 *                  This now uses the MovingAverage library. It gives you much quicker ways of changing the smoothing of the data and thus sensitivity
 *                  Also - setting a threshold wasnt reliable. Now its a % change from the average. 
 *                  This needs testing - but makes sense in a accelerometer where a movement may be occuring slowly down a chair - but you still want to 
 *                    see a "sharp" (could be subtle) increase. 
 *                  Also - we can switch between measuring x, y, z - or measure all if you so wish. 
 *                  Simply turn on/off the flags below for measurex, measurey, measurez. 
 *                  
 *                  Its easiest if you view the data to see which measurement to turn on/off - and you can get an idea of the % change you require
 *                  Do this by turning on debug=true and debugPlot and use with SerialPlot (https://hackaday.io/project/5334-serialplot-realtime-plotting-software)
 *                  to generate live data
 *                  
 *                  Be aware: If you want to really change the "sensitivity" you have to bear in mind a few things:
 *                  1. The sample rate. See the delay at the very bottom of this code. Want to make it faster? Lower that delay
 *                  2. The 'MovingAverage averagex(0.3);' lines (and see y, z also). Basically read that comment. Its a co-efficient
 *                  3. Change the threshold % change variable
 *                  
 *                  One comment: We could make this simpler by putting a trigger for a acceleration in m/s2. Its certainly an idea. Bear in in mind though its not massively 
 *                               different. The raw numbers are just converted with a bit of maths. The % change feels better to me. But I could easily be persuaded :) 
 */

//******************************************************************
#include <Wire.h>
#include <Adafruit_MMA8451.h> //https://github.com/adafruit/Adafruit_MMA8451_Library
#include <Adafruit_Sensor.h> //https://github.com/adafruit/Adafruit_Sensor
#include <Keyboard.h>
#include <MovingAverage.h> //https://github.com/sofian/MovingAverage
//#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary
Adafruit_MMA8451 mma = Adafruit_MMA8451();
// PUT BUZZER ON PIN 10. MMA8451 is then wired to SDA/SCL etc
//******************************************************************

/* settings - setup. Dont touch */
int avgx,avgy,avgz, diffx,diffy,diffz, counter, switchValue = 0;
boolean swOn = false;
int onTmr;

/* settings - Feel free to change */
boolean debug = true;
boolean debugPlot = true;
const int buzzerPin = 10;
const int threshold = 70; //% CHANGE
boolean measurex = true;
boolean measurey = false;
boolean measurez = false;

/* Want to change the sensitivity of any of the axis? Change this. from 0-1 0=A lot of smoothing and 1= Closer to the actual numbers 
 *  Note - this is a weighting co-efficient. 
 *  A moving average works using a weighting decrease coefficient (α), 
 *  a value between 0 and 1 that is used to mix the current value with the trace of all preceding values. 
 *  A higher α discounts older observations faster.
 *  
 *  For more info read: https://github.com/sofian/MovingAverage#create-an-instance 
*/

MovingAverage averagex(0.3);
MovingAverage averagey(0.3);
MovingAverage averagez(0.3); 

/* settings - setup. Dont touch */
char keyCode = KEY_F1;      //For the correct code see http://www.asciitable.com (Use dec column) or for modifiers use https://www.arduino.cc/en/Reference/KeyboardModifiers 
const int volume = 5;         // 128 will be loudest, go down to about 10. 0=Off. 

/* Function defines what happens when a switch is on */
void turnOnSwitch(){
  swOn = true;
  analogWrite(buzzerPin,volume);   // sound
}


void turnOffSwitch(){
  swOn = false;
  analogWrite(buzzerPin,0);   // sound
}

int getDiffValue(float accelRaw, float avg){
  return abs(float(accelRaw) / float(avg) - 1)*100;
}


void setup() {
  /* set up averages */
  averagex.reset( analogRead(0) );
  averagey.reset( analogRead(0) );
  averagez.reset( analogRead(0) );
  
  Keyboard.begin();
  pinMode(buzzerPin, OUTPUT); // Get buzzer ready
  delay(5000);     
  Serial.begin(115200);
  Serial.println("AccelSwitch v2");
  if (! mma.begin()) {
    Serial.println("Couldnt start Accelerometer");
    while (1);
  }
  Serial.println("MMA8451 found!");
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  //Startup sound
  for(counter = 2;counter <= 50;counter++) {
    analogWrite(buzzerPin,counter); delay(20); analogWrite(10,0);  
  }
  Serial.println("AccelSwitch Ready");
}

/* Main */

void loop() {
  mma.read();
  // Get Averages to date
  avgx = averagex.get(); avgy = averagey.get(); avgz = averagez.get();
  // Get difference from Average
  diffx = getDiffValue(float(mma.x), float(avgx)); diffy = getDiffValue(float(mma.y), float(avgy)); diffz = getDiffValue(float(mma.z), float(avgz));
  
     if (debug){
      if (debugPlot){
        /* so this outputs
         * rawx,avgx,rawy,avgy,rawz,avgz,swOn, diffx, diffy, diffz
         * second line - with tabs. third line - spaces
         * 
         * NB: Works nicely with SerialPlot: https://hackaday.io/project/5334-serialplot-realtime-plotting-software
         * 
         */
        if (swOn) { switchValue = 1000; } else { switchValue = -1000; }
        //Serial.print(mma.x);Serial.print("\t");Serial.print(avgx);Serial.print("\t");Serial.print(mma.y);Serial.print("\t");Serial.print(avgy);Serial.print("\t");Serial.print(mma.z);Serial.print("\t");Serial.print(avgz);Serial.print("\t");Serial.print(switchValue);Serial.print("\t");Serial.print(diffx);Serial.print("\t");Serial.print(diffy);Serial.print("\t");Serial.print(diffz);Serial.println();
        Serial.print(mma.x);Serial.print(" ");Serial.print(avgx);Serial.print(" ");Serial.print(mma.y);Serial.print(" ");Serial.print(avgy);Serial.print(" ");Serial.print(mma.z);Serial.print(" ");Serial.print(avgz);Serial.print(" ");Serial.print(switchValue);Serial.print(" ");Serial.print(diffx);Serial.print(" ");Serial.print(diffy);Serial.print(" ");Serial.print(diffz);Serial.println();
      }

      if (debugPlot == false){
        Serial.print("Raw:\t");Serial.print(mma.x);Serial.print("\t");Serial.print(mma.y);Serial.print("\t");Serial.print(mma.z);Serial.println();
        Serial.print("Avg:\t");Serial.print(avgx);Serial.print("\t");Serial.print(avgy);Serial.print("\t");Serial.print(avgz);Serial.println();
        Serial.print("Diff:\t");Serial.print(diffx);Serial.print("\t");Serial.print(diffy);Serial.print("\t");Serial.print(diffz);Serial.println();
        Serial.print("Switch On?:\t");Serial.print(swOn);
        Serial.println();
      }
     }  

      if (swOn){
        if (measurex && diffx <= float(threshold)) {
          if (debug && debugPlot == false ){ Serial.println("Turning off x"); }
          turnOffSwitch();
        }
  
        if (measurey && diffy <= float(threshold)) {
          if (debug && debugPlot == false ){ Serial.println("Turning off y"); }
          turnOffSwitch();
        }
  
        if (measurez && diffz <= float(threshold)) {
          if (debug && debugPlot == false ){ Serial.println("Turning off z"); }
          turnOffSwitch();
        }
      } else  {
      
        if (measurex && diffx <= float(threshold)) {
          if (debug && debugPlot == false ){ Serial.println("Turning on x"); }
          turnOnSwitch();
        }
  
        if (measurey && diffy > float(threshold)) {
         if (debug && debugPlot == false ){ Serial.println("Turning on y"); }
         turnOnSwitch();
        }
  
        if (measurez && diffz > float(threshold)) {
          if (debug && debugPlot == false ){ Serial.println("Turning on z");}
          turnOnSwitch();
        }
      }
  // Update Averages
  averagex.update( mma.x ); averagey.update( mma.y ); averagez.update( mma.z );
  delay(10);
}
