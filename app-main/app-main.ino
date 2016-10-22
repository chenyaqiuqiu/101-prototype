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
#define   TIMER_1_5S  15
#define   TIMER_2S    20
#define   TIMER_3S    30
#define   TIMER_6S    60

// BAT 4.2V is full 800 = 860
#define   BAT_POWER_FULL              820
// BAT 3.8V is preWarnning
#define   BAT_POWER_PREWARNNING       780
#define   BAT_POWER_ALARM             200

// check BAT is checking ??
#define   BAT_CHARGING_REFVALUE_HIGH  500

#define   SAMPLE_BUTTON_PUSHED_VOLTAGE      980
#define   SAMPLE_BUTTON_RELEASED_VOLTAGE    100

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

int temData = 0;
int i;

int A3Value;
int A1Value;

int batteryPowerStatus;
int sampleButtonVoltage;

int heartRate[HEARTRATE_TIMES_PERSEC];
int heartRateAvrSec;

int chargingTimer = 0;
int sampleButtonPushedTimer = 0;
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
  // add you code here to send data to APP
}

static void calculateHeartRate(void)
{
  // heartBeat
  for (i = 0; i < HEARTRATE_TIMES_PERSEC; i++) {
    heartRateAvrSec += heartRate[i];
    heartRateAvrSec = heartRateAvrSec / 10;
  }
}

static void updateBatteryStatus(void)
{
    int A1_BatteryVoltage = 0;

    // get Battery Voltage by A1 Pin
    A1_BatteryVoltage = analogRead(A1);

    if (A1_BatteryVoltage > BAT_POWER_FULL) {
      batteryPowerStatus =  full;
    } else if (A1_BatteryVoltage > BAT_POWER_PREWARNNING
               && A1_BatteryVoltage <= BAT_POWER_FULL) {
      batteryPowerStatus = preWarnning;
    } else {
      batteryPowerStatus = alarm;
    }
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

static void printIntT(unsigned long val, int len)
{
  char sz[32] = {0};
  
  sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

void loop() {
    printIntT(10, 5);
    while(1);
    // update Battery every 0.1s
    updateBatteryStatus();

    if (arduinoStatus == normal) {
      LedsShowBATStatus(batteryPowerStatus);
    }

    // battaryStatus = preWarnning;
    // A1Value = 1000;
    // charging battary
    A3Value = analogRead(A3);
    if (A3Value > BAT_CHARGING_REFVALUE_HIGH) {
      // update arduino status to battery is charging
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

    // dataSample Button checking
    sampleButtonVoltage = analogRead(A2);

    if (sampleButtonVoltage > SAMPLE_BUTTON_PUSHED_VOLTAGE) {
      sampleButtonPushedTimer++;

      if (sampleButtonPushedTimer >= TIMER_1_5S) {
        if (dataSampleStart == true) {
          dataSampleStart = false;
          sampleButtonPushedTimer = 0;
        }
      }

      if (sampleButtonPushedTimer >= TIMER_2S) {
        dataSampleStart = true;
      }

    } else {
      dataSampleStart = false;
      sampleButtonPushedTimer = 0;
    }


    // data Sample
    dataSampleStart = true;
    batteryPowerStatus = preWarnning;

   if (dataSampleStart == true) {
      arduinoStatus = dataSample;
      heartRate[dataSampleTimer++] = analogRead(A0);

      if (dataSampleTimer == TIMER_1S) {
        calculateHeartRate();
        Serial.print("beartRate:");
        Serial.print(heartRateAvrSec);
        Serial.print("\r\n");
        sdWriteHeartBeat(heartRateAvrSec);

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
  } else {
    arduinoStatus = normal;
  }

  // BLE sending
  if (arduinoStatus == normal) {
    if (sendDataToAppFlag) {
      // BLE is connected and send data to App by BLE
      arduinoStatus = bleSendingData;
    }
  }
  delay(100);
}

