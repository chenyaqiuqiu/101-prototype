#include <BMI160.h>
#include <CurieIMU.h>
#include "SdCard.h"
#include "StepCount.h"
#include "Gyro.h"
#include "Accelerometer.h"
#include "ThreeColorLeds.h"
#include "CurieTimerOne.h"
#include "Gps.h"

#define APP_DEBUG 1

#ifdef APP_DEBUG
#define AppDebug(x) Serial.println(x)
#else
#define AppDebug(x) {}
#endif

// 100ms
#define   LOOP_INTERVAL   100
#define   TIMER_0S    0
#define   TIMER_1S    (1000 / LOOP_INTERVAL)
#define   TIMER_1_5S  (1500 / LOOP_INTERVAL)
#define   TIMER_2S    (2000 / LOOP_INTERVAL)
#define   TIMER_3S    (3000 / LOOP_INTERVAL)
#define   TIMER_6S    (6000 / LOOP_INTERVAL)

// BAT 4.2V is full 800 = 860
#define   BAT_POWER_FULL              820
// BAT 3.8V is preWarnning
#define   BAT_POWER_PREWARNNING       780
#define   BAT_POWER_ALARM             200
// check BAT is checking ??
#define   BAT_CHARGING_REFVALUE_HIGH        500

#define   SAMPLE_BUTTON_PUSHED_VOLTAGE      980
#define   SAMPLE_BUTTON_RELEASED_VOLTAGE    100

#define   HEARTRATE_TIMES_PERSEC    10
#define   BLE_LEDS_INTERVAL_TIMERS    100000

static const int debugBaud = 9600;
static const int gpsBaud = 9600;
static const int sdCardCsPin = 2;

static bool sendDataToAppFlag = false;
static bool BATCharging = false;
static bool DataSampling = false;
static bool BLESending = false;

int temData = 0;
int i;

int A3Value;

int batteryPowerStatus;
int sampleButtonVoltage;

int heartRate[HEARTRATE_TIMES_PERSEC];
int heartRateAvrSec;

int chargingTimer = 0;
int sampleButtonPushedTimer = 0;
int dataSampleTimer = 0;

int dataSampleStart = 0;
bool dataSampleLeds = true;
int bleCounterIndex = 0;

long stepsCounter;

float aclX;
float aclY;
float aclZ;
float gX;
float gY;
float gZ;

int year_lastMs;
int month_lastMs;
int day_lastMs;
int hour_lastMs;
int minute_lastMs;
int second_lastMs;

void sendDataToApp(void)
{
  // read SD data from SD card and Send to App
  sdReadheartRateFile();
  
  // add you code here to send data to APP
}

void timedBlinkIsr()   // callback function when interrupt is asserted
{
  if (bleCounterIndex == 0 | bleCounterIndex == 3) {
    LedsShowBATStatus(batteryPowerStatus);
  } else {
    turnOffLeds();  
  }
  bleCounterIndex++;
  if (bleCounterIndex == 10)
    bleCounterIndex = 0;
}

static void calculateHeartRate(void)
{
  // heartBeat
  for (i = 0; i < HEARTRATE_TIMES_PERSEC; i++) {
    heartRateAvrSec += heartRate[i];
    heartRateAvrSec = heartRateAvrSec / HEARTRATE_TIMES_PERSEC;
  }
}

void getAndWriteGpsData(void)
{
  int distance;
  float gpsSpeed;
  int alMeters;
  int cur_year, cur_month, cur_day, cur_hour, cur_minute, cur_second;

  getGpsDistance(&distance);
  getGpsSpeed(&gpsSpeed);
  getGpsAltitude(&alMeters);
  getGpsTime(&cur_year, &cur_month, &cur_day, &cur_hour, &cur_minute, &cur_second);

  year_lastMs = cur_year;
  month_lastMs = cur_month;
  day_lastMs = cur_day;
  hour_lastMs = cur_hour;
  minute_lastMs = cur_minute;
  second_lastMs = cur_second;

  sdWriteDistance(distance);
  sdWriteSpeed(gpsSpeed);
  sdWriteAltitude(alMeters);
  //sdWriteDate();
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
  AppDebug("Initializing is Done.");
}

void loop() {
  //sdReadLine();
  // update Battery every 0.1s
  updateBatteryStatus();
  batteryPowerStatus =  full;
  A3Value = analogRead(A3);
  A3Value = BAT_CHARGING_REFVALUE_HIGH - 1;

  if (A3Value > BAT_CHARGING_REFVALUE_HIGH) {
    BATCharging = true;
    if (chargingTimer == TIMER_0S) {
      Serial.print("show charging status\r\n");
      LedsShowBATStatus(batteryPowerStatus);
    } else if (chargingTimer == TIMER_3S) {
      turnOffLeds();
      Serial.print("turnOff all leds\r\n");
    }

    if (chargingTimer == TIMER_6S) {
      chargingTimer = TIMER_0S;
    }
    else
      chargingTimer++;
  } else {
    BATCharging = false;
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
  // Debug Propose
  dataSampleStart = true;
  if (dataSampleStart == true) {
    DataSampling = true;
    heartRate[dataSampleTimer++] = analogRead(A0);

    if (dataSampleTimer == TIMER_1S) {
      calculateHeartRate();
      AppDebug("beartRate:");
      AppDebug(heartRateAvrSec);
      AppDebug("\r\n");
      sdWriteHeartRate(heartRateAvrSec);

      stepsCounter = getStepCounts();  //steps Count
      AppDebug("counter: ");
      AppDebug(stepsCounter);
      sdWriteStepCount(stepsCounter);

      getGyroValue(&gX, &gY, &gZ);
      getAccelrometerValue(&aclX, &aclY, &aclZ);
      sdWriteAclAndGyro(gX, gY, gZ, aclX, aclY, aclZ);
      //getAndWriteGpsData();

      dataSampleTimer = 0;
      dataSampleLeds = !dataSampleLeds;
    }
    
    if (dataSampleLeds) {
      LedsShowBATStatus(batteryPowerStatus);
    } else {
      turnOffLeds();
    }
  } else {
    DataSampling = false;
  }

// open the below comments to debug BLE
//sendDataToAppFlag = false;
  // BLE sending
  if (sendDataToAppFlag) {
    // BLE is connected and send data to App by BLE
    BLESending = true;
    CurieTimerOne.start(BLE_LEDS_INTERVAL_TIMERS, &timedBlinkIsr);  // set timer and callback
  } else {
    CurieTimerOne.stop();
    BLESending = false;
  }

  if (!(BATCharging || DataSampling || BLESending)) {
    LedsShowBATStatus(batteryPowerStatus);
  }

  delay(LOOP_INTERVAL);
}

