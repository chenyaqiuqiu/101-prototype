#include <BMI160.h>
#include <CurieIMU.h>

#include "SdCard.h"
#include "StepCount.h"
#include "Gyro.h"
#include "Accelerometer.h"
#include "ThreeColorLeds.h"
#include "Gps.h"

#define   TIMER_0S    0
#define   TIMER_1S    10
#define   TIMER_2S    20
#define   TIMER_3S    30
#define   TIMER_6S    60

#define   BAT_POWER_FULL              800
#define   BAT_POWER_PREWARNNING       400
#define   BAT_POWER_ALARM             200

#define   BAT_CHARGING_REFVALUE_HIGH  500

#define   SAMPLE_BUTTON_REFVALUE_HIGH   980
#define   SAMPLE_BUTTON_REFVALUE_LOW    100

#define   HEARTRATE_TIMES_PERSEC    10

enum _runningStatus {
  normal = 0,
  batteryCharging,
  bleSendingData,
  dataSample
};

static const int debugBaud = 9600;
static const int gpsBaud = 9600;

static const int sdCardCsPin = 4;

static bool sendDataToAppFlag = false;
static int arduinoStatus = normal;

int batteryPowerStatus;

int A0Value = 0;
int A1Value = 0;
int A2Value = 0;
int A3Value = 0;

int heartBeat[HEARTRATE_TIMES_PERSEC];
int heartBeatAvrSec;

int temData = 0;
int i;

int chargingTimer = 0;
int dataSampleSignalTimer = 0;
int dataSampleTimer = 0;

int dataSampleStart = 0;
bool dataSampleLeds = 1;

long stepsCounter;

float aclX;
float aclY;
float aclZ;
float gX;
float gY;
float gZ;

void sendDataToApp(void)
{
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(debugBaud);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  CurieIMU.begin();
  
  gpsSetup(gpsBaud);
  sdCardSetup(sdCardCsPin);
  CurieStepCountSetup();
  accelerometerSetup();
  gyroSetup();
  ledsSetup();
  Serial.print("Initializing is Done.");
}

static void updateBattaryStatus(int refValue)
{
  if (refValue > BAT_POWER_FULL) {
    batteryPowerStatus =  full;
  } else if (refValue > BAT_POWER_PREWARNNING
             && refValue <= BAT_POWER_FULL) {
    batteryPowerStatus = preWarnning;
  } else {
    batteryPowerStatus = alarm;
  }
}

void loop() {
sdWriteAclAndGyro(1.1, 1.2, 1.3, 1.4, 1.6, 1.7);
sdWriteAclAndGyro(2.1155, 2.2, 2.3, 2.4, 2.6, 2.7);

while(1);

  A1Value = analogRead(A1);
  A2Value = analogRead(A2);
  A3Value = analogRead(A3);

  updateBattaryStatus(A2Value);

  if (arduinoStatus == normal) {
    LedsShowBATStatus(batteryPowerStatus);
  }

  // battaryStatus = preWarnning;
  // A1Value = 1000;

  if (A1Value > BAT_CHARGING_REFVALUE_HIGH) {
    // charging battary
    arduinoStatus = batteryCharging;
    if (chargingTimer == TIMER_0S) {
      LedsShowBATStatus(batteryPowerStatus);
    } else if (chargingTimer == TIMER_3S) {
      turnOffLeds();
      //Serial.print("turnOff all leds");
    }

    if (chargingTimer == TIMER_6S) {
      chargingTimer = TIMER_0S;
    }
    else
      chargingTimer++;
  } else {
    arduinoStatus = normal;
  }

  // dataSample checking
  if (A3Value < SAMPLE_BUTTON_REFVALUE_LOW) {
    dataSampleSignalTimer++;
    if (dataSampleSignalTimer >= TIMER_2S) {
      dataSampleStart = true;
    }
  } else if (A3Value > SAMPLE_BUTTON_REFVALUE_HIGH) {
    if (dataSampleStart == true) {
      dataSampleStart = false;
      dataSampleSignalTimer = 0;
    }
  }
  // data Sample
  dataSampleStart = true;
  batteryPowerStatus = preWarnning;

  if (dataSampleStart == 1) {
    arduinoStatus = dataSample;
    A0Value = analogRead(A0);

    heartBeat[dataSampleTimer++] = A0Value;

    if (dataSampleTimer == TIMER_1S) {
      // heartBeat
      for (i = 0; i < 10; i++) {
        heartBeatAvrSec += heartBeat[i];
        heartBeatAvrSec = heartBeatAvrSec / 10;
      }

      Serial.print("beartBeat:");
      Serial.print(heartBeatAvrSec);
      Serial.print("\r\n");
      sdWriteHeartBeat(heartBeatAvrSec);
      
      stepsCounter = getStepCounts();  //steps Count
      Serial.print("counter: ");
      Serial.println(stepsCounter);
      sdWriteStepCount(stepsCounter);
            
      getGyroValue(&gX, &gY, &gZ);
      getAccelrometerValue(&aclX, &aclY, &aclZ);
      sdWriteAclAndGyro(gX, gY, gZ, aclX, aclY, aclZ);

      dataSampleTimer = 0;
      dataSampleLeds = !dataSampleLeds;
    }

    // Serial.print(dataSampleCount);
    // Serial.print("\r\n");
    //Serial.print(dataSampleLeds);
    // Serial.print("\r\n");

    if (dataSampleLeds) {
      LedsShowBATStatus(batteryPowerStatus);
    } else {
      turnOffLeds();
    }
  }
  
    // BLE sending
    if (arduinoStatus == normal)
      if (sendDataToAppFlag) {
        // BLE is connected and send data to App by BLE
        arduinoStatus = bleSendingData;
      }

  delay(100);
}

