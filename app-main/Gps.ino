#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define GPS_DEBUG 1

#ifdef GPS_DEBUG
#define GpsDebug(x) Serial.println(x)
#else
#define GpsDebug(x) {}
#endif

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
  GpsDebug("GPS Serial Band: ");
  GpsDebug(gpsBaud);
}

int gpsScan(void)
{
  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS data received: check wiring"));
    return -1;
  }

  return 0;
}

void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

int getGpshdopAndsateNum(int *satellitesNum, int *hdop)
{
  smartDelay(0);
  if (gps.satellites.isValid()) {
    *satellitesNum = gps.satellites.value();
    *hdop = gps.hdop.value();
    GpsDebug("SatellitesNum : ");
    GpsDebug(*satellitesNum);
    return 0;
  }

  GpsDebug("Hdop&sateNum is not valid\r\n");
  return -1;
}

int getGpslngAndlatValue(double *lat, double *lng, int *age)
{
  smartDelay(0);
  if (gps.location.isValid()) {
    *lat =  gps.location.lat();
    *lng = gps.location.lng();
    lastLat = *lat;
    lastLng = *lng;

    *age = gps.location.age();
    return 0;
  }
  GpsDebug("Location is not valid\r\n");
  return -1;
}

int getGpsAltitude(float *altitudeMeters)
{
  smartDelay(0);
  if (gps.altitude.isValid()) {
    *altitudeMeters = gps.altitude.meters();
    GpsDebug("meters: ");
    GpsDebug(*altitudeMeters);
    return 0;
  }
  GpsDebug("Altitude is not valid\r\n");
  return -1;
}

int getGpsTime(int *year, int *month, int *day, int *hour, int *minute, int *second)
{
  smartDelay(0);
  if (gps.date.isValid()) {
    *year = gps.date.year();
    *month = gps.date.month();
    *day = gps.date.day();

    GpsDebug("Date:");
    GpsDebug("year: ");
    GpsDebug(*year);
    GpsDebug("month: ");
    GpsDebug(*month);
    GpsDebug("day: ");
    GpsDebug(*day);

  } else {
    GpsDebug("Date is not valid\r\n");
    return -1;
  }

  if (gps.time.isValid()) {
    *hour = gps.time.hour();
    *minute = gps.time.minute();
    *second = gps.time.second();

    GpsDebug("Time: ");
    GpsDebug("hour: ");
    GpsDebug(*hour);
    GpsDebug("minute: ");
    GpsDebug(*minute);
    GpsDebug("second: ");
    GpsDebug(*second);
  } else {

    GpsDebug("Time is not valid\r\n");
    return -1;
  }

  return 0;
}

void getGpsDistance(int *distance)
{
  smartDelay(0);
  *distance = (int)TinyGPSPlus::distanceBetween(
                gps.location.lat(),
                gps.location.lng(),
                lastLat,
                lastLng) / 1000;

  Serial.print("Distance: ");
  Serial.print(*distance);
  Serial.print("\r\n");
}

int getGpsSpeed(float *speed)
{
  smartDelay(0);
  if (gps.speed.isValid()) {
    *speed = gps.speed.kmph();
    GpsDebug("Speed: ");
    GpsDebug(*speed);
    return 0;
  }

  GpsDebug("speed is not valid\r\n");
  return -1;
}
