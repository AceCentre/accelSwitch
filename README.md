# AccelSwitch

## What does this do?

Its presses a key (or joystick button push) - when the accelerometer is activated above a threshold. 
It can be used as a switch for assistive technology

## What do you need?

- Arduino 
- [Adafruit MMA8451](https://learn.adafruit.com/adafruit-mma8451-accelerometer-breakout/overview)
- ....

## Notes

### If using the GY-45 MMA8451

If you are using one of [these](https://www.ebay.co.uk/itm/GY-45-MMA8451-3-Axis-Accelerometer-Module-for-Arduino-RPi-ESP8266-etc/272667139511?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649
) You will need to modify [line 39](https://github.com/adafruit/Adafruit_MMA8451_Library/blob/dbb33718e1567f63e09c62a0e4de8b9fb6925371/Adafruit_MMA8451.h#L39) in the header of the Adafruit library. 
 
# Who made this 

- Gerard Cullen: http://rhn.org.uk
