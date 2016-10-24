/*
     RGB Leds
*/

#include "ThreeColorLeds.h"

void ledsSetup(void)
{
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  
  turnOffLeds();
}

void setLedsRGB(int red, int green, int blue)
{
  analogWrite(RED_LED,   red);
  analogWrite(GREEN_LED, green);
  analogWrite(BLUE_LED,  blue);
}

void LedsShowBATStatus(int battaryStatus)
{
  switch (battaryStatus) {
    case full:
      setLedsRGB(0, 255, 0);
      
      break;
    case preWarnning:
      setLedsRGB(0, 0, 255);
      //Serial.print("warnning\r\n");
      break;
    case alarm:
      setLedsRGB(255, 0, 0);
      break;
    default:
      setLedsRGB(0, 0, 0);
  }
}

void turnOffLeds(void)
{
  setLedsRGB(0, 0, 0);
}
