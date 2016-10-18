
#include <CurieIMU.h>

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
  Serial.print("a:\t");
  Serial.print(*ax);
  Serial.print("\t");
  Serial.print(*ay);
  Serial.print("\t");
  Serial.print(*az);
  Serial.println();
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;

  return a;
}
