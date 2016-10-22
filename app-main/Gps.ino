#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int gpsRxPin = 4;
static const int gpsTxPin = 3;

double lastLat = 0;
double lastLng = 0;

// The TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial ss(gpsRxPin, gpsTxPin);
// The serial connection to the GPS device

void gpsSetup(int gpsBaud)
{
  ss.begin(gpsBaud);
  Serial.print(gpsBaud);
}

int getGpshdopAndsateNum(int *satellitesNum, int *hdop)
{
  if (gps.satellites.isValid()) {
    *satellitesNum = gps.satellites.value();
    *hdop = gps.hdop.value();
    return 0;
  }

  return -1;
}
int getGpslngAndlatValue(double *lat, double *lng, int *age)
{
  if (gps.location.isValid()) {
    *lat =  gps.location.lat();
    *lng = gps.location.lng();
    lastLat = *lat;
    lastLng = *lng;

    *age = gps.location.age();
    return 0;
  }

  return -1;
}

int getGpsAltitude(int *altitudeMeters)
{
  if (gps.altitude.isValid()) {
    *altitudeMeters = gps.altitude.meters();
    return 0;
  }

  return -1;
}

int getGpsTime(int *year, int *month, int *day, int *hour, int *minute, int *second)
{
  if (gps.date.isValid()) {
    *year = gps.date.year();
    *month = gps.date.month();
    *day = gps.date.day();
  } else {
    return -1;
  }

  if (gps.time.isValid()) {
    *hour = gps.time.hour();
    *minute = gps.time.minute();
    *second = gps.time.second();
  } else {
    return -1;
  }

  return 0;
}

void getGpsDistance(int *distance)
{
  *distance = (int)TinyGPSPlus::distanceBetween(
                gps.location.lat(),
                gps.location.lng(),
                lastLat,
                lastLng) / 1000;
}

int getGpsSpeed(float *speed)
{
  if (gps.speed.isValid()) {
    *speed = gps.speed.kmph();
    return 0;
  }
  
  return -1;
}
