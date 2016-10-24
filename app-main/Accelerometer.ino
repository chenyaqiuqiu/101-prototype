
#include <CurieIMU.h>

#define ACL_DEBUG 1

#ifdef ACL_DEBUG
#define AclDebug(x) Serial.println(x) 
#else
#define AclDebug(x) {}
#endif

void accelerometerSetup() {
  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
}

void getAccelrometerValue(float *ax, float *ay, float *az) {
  int axRaw, ayRaw, azRaw;

  // read raw accelerometer measurements from device
  CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

  // convert the raw accelerometer data to G's
  *ax = convertRawAcceleration(axRaw);
  *ay = convertRawAcceleration(ayRaw);
  *az = convertRawAcceleration(azRaw);

  // display tab-separated accelerometer x/y/z values
  AclDebug("a:\t");
  AclDebug(*ax);
  AclDebug("\t");
  AclDebug(*ay);
  AclDebug("\t");
  AclDebug(*az);
  AclDebug();
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;

  return a;
}
