#ifndef THREE_COLOR_LED_H
#define THREE_COLOR_LED_H

#define RED_LED     3
#define GREEN_LED   5
#define BLUE_LED    6

enum _battaryStatus {
  full = 1,
  preWarnning,
  alarm
};

void ledsSetup(void);
void LedsShowBATStatus(int battaryStatus);
void setLedsRGB(int red, int green, int blue);

#endif
