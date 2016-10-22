#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int gpsRxPin = 4;
static const int gpsTxPin = 3;

static void printGpsDateTime(TinyGPSDate &d, TinyGPSTime &t);

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


static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void getIntValue(unsigned long val, bool valid, int len, char *result)
{
  if (valid)
    sprintf(result, "%ld", val);
  result[len] = 0;
  for (int i = strlen(result); i < len; ++i)
    result[i] = ' ';
  if (len > 0)
    result[len - 1] = ' ';
  Serial.print(result);
  smartDelay(0);
}

static void getFloatValue(float val, bool valid, int len, int prec, char *result)
{
  if (valid)
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
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

int getGpsAltitude(float *altitudeMeters)
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

void getGpsDistance(unsigned long *distance)
{
  *distance = (unsigned long)TinyGPSPlus::distanceBetween(
                gps.location.lat(),
                gps.location.lng(),
                lastLat,
                lastLng) / 1000;
  printInt(*distance, gps.location.isValid(), 9);

}

void getGpsValue()
{
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printGpsDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "*** ", 6);

  unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT,
      LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT,
      LONDON_LON);

  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printGpsDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}
