#include <BMI160.h>
#include <CurieIMU.h>

#include "SdCard.h"
#include "StepCount.h"
#include "Gyro.h"
#include "Accelerometer.h"
#include "ThreeColorLeds.h"

enum _runningStatus {
  normal = 0,
  batteryCharging,
  bleSendingData,
  dataSample
};

static bool sendDataToAppFlag = false;
static int arduinoStatus = normal;

int battaryStatus = 0;
int A0Value = 0;
int A1Value = 0;
int A2Value = 0;
int A3Value = 0;

int heartBeat[10];
int heartBeatAvrSec;
int temData = 0;
int i;

int chargingCount = 0;

int dataSampleSignalCount = 0;
int dataSampleCount = 0;
int dataSampleStart = 0;
bool dataSampleLeds = 1;

float aclX;
float aclY;
float aclZ;

float gX;
float gY;
float gZ;

long stepsCounter;

void sendDataToApp(void)
{
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  CurieIMU.begin();
  sdCardSetup();
  setupCurieStepCount();
  accelerometerSetup();
  gyroSetup();
  ledsSetup();
  Serial.print("Initializing is Done.");
}

void fullfillBattaryStatus(int refValue)
{
  if (refValue > 800) {
    battaryStatus =  full;
  } else if (refValue > 400 && refValue <= 800) {
    battaryStatus = preWarnning;
  } else {
    battaryStatus = alarm;
  }
}

void loop() {

  sdWriteHeartBeat(1000);
  while (1);
  A1Value = analogRead(A1);
  A2Value = analogRead(A2);
  A3Value = analogRead(A3);
  fullfillBattaryStatus(A2Value);

  if (arduinoStatus == normal) {
    LedsShowBATStatus(battaryStatus);
  }

  // battaryStatus = preWarnning;
  // A1Value = 1000;

  if (A1Value > 500) {
    // charging battary
    arduinoStatus = batteryCharging;
    if (chargingCount == 0) {
      LedsShowBATStatus(battaryStatus);
    } else if (chargingCount == 30) {
      setLedsRGB(0, 0, 0);
      //Serial.print("turnOff all leds");
    }

    if (chargingCount == 60) {
      chargingCount = 0;
    }
    else
      chargingCount++;
  } else {
    arduinoStatus = normal;
  }

  // dataSample checking
  if (A3Value < 100) {
    dataSampleSignalCount++;
    if (dataSampleSignalCount >= 20) {
      dataSampleStart = 1;
    }
  } else if (A3Value > 980) {
    if (dataSampleStart == 1) {
      dataSampleStart = 0;
      dataSampleSignalCount = 0;
    }
  }
  // data Sample
  dataSampleStart = 1;
  battaryStatus = preWarnning;

  if (dataSampleStart == 1) {
    arduinoStatus = dataSample;
    A0Value = analogRead(A0);

    heartBeat[dataSampleCount++] = A0Value;

    if (dataSampleCount == 10) {
      // heartBeat
      for (i = 0; i < 10; i++) {
        heartBeatAvrSec += heartBeat[i];
        heartBeatAvrSec = heartBeatAvrSec / 10;
      }

      // Serial.print("beartBeat:");
      // Serial.print(heartBeatAvrSec);
      // Serial.print("\r\n");

      stepsCounter = getStepCounts();  //steps Count
      Serial.print("counter: ");
      Serial.println(stepsCounter);

      getGyroValue(&gZ, &gY, &gZ);
      getAccelrometerValue(&aclX, &aclY, &aclZ);
      dataSampleCount = 0;
      dataSampleLeds = !dataSampleLeds;
    }

    // Serial.print(dataSampleCount);
    // Serial.print("\r\n");
    //Serial.print(dataSampleLeds);
    // Serial.print("\r\n");

    if (dataSampleLeds) {
      LedsShowBATStatus(battaryStatus);
    } else {
      setLedsRGB(0, 0, 0);
    }
  }
  /*
    // BLE sending
    if (arduinoStatus == standBy)
      if (sendDataToAppFlag) {
        // BLE is connected and send data to App by BLE
        arduinoStatus = bleSendingData;
      }
  */
  delay(100);
}

