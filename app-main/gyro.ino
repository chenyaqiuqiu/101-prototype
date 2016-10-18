#include "CurieIMU.h"

void gyroSetup() {
  Serial.print("Starting Gyroscope calibration...");
  CurieIMU.autoCalibrateGyroOffset();
  Serial.println("Gyro Done");
  // Set the accelerometer range to 250 degrees/second
  CurieIMU.setGyroRange(250);
}

void getGyroValue(float *gx, float *gy, float *gz) {
  int gxRaw, gyRaw, gzRaw;         // raw gyro values
  // read raw gyro measurements from device
  CurieIMU.readGyro(gxRaw, gyRaw, gzRaw);

  // convert the raw gyro data to degrees/second
  *gx = convertRawGyro(gxRaw);
  *gy = convertRawGyro(gyRaw);
  *gz = convertRawGyro(gzRaw);

  // display tab-separated gyro x/y/z values
  Serial.print("g:\t");
  Serial.print(*gx);
  Serial.print("\t");
  Serial.print(*gy);
  Serial.print("\t");
  Serial.print(*gz);
  Serial.println();
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  float g = (gRaw * 250.0) / 32768.0;

  return g;
}
