#include <BMI160.h>
#include <CurieIMU.h>
#include "SdCard.h"
#include "StepCount.h"
#include "Gyro.h"
#include "Accelerometer.h"
#include "ThreeColorLeds.h"
#include "CurieTimerOne.h"
#include "Gps.h"
#include "BLE.h"

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

// BAT 4.15V is full
#define   BAT_POWER_FULL              850
// BAT 3.64V is preWarnning status
#define   BAT_POWER_PREWARNNING       746
// BAT 3.5V is Alarm status
#define   BAT_POWER_ALARM             716

#define   SAMPLE_BUTTON_PUSHED_VOLTAGE      980

#define   HEARTRATE_TIMES_PERSEC    10
#define   BLE_LEDS_INTERVAL_TIMERS    100000

static const int debugBaud = 9600;
static const int gpsBaud = 9600;
static const int sdCardCsPin = 2;

// 保存电池电量的值
int currentBATVoltage;
int lastBATVoltage;

// Arduino101 运行状态
static bool BATCharging = false;
static bool DataSampling = false;
static bool BLESending = false;

int temData = 0;
int i;

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
int distanceAvrSec;
int distance;

float aclX;
float aclY;
float aclZ;
float gX;
float gY;
float gZ;

int lastYear;
int lastMonth;
int lastDay;
int lastHour;
int lastMinute;
int lastSecond; 

void sendDataToApp(void)
{
  // read SD data from SD card and Send to App
  sdReadheartRateFile();

  // add you code here to send data to APP
}

void timedBlinkIsr()   // callback function when interrupt is asserted
{
  if (bleCounterIndex == 0 || bleCounterIndex == 3) {
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

void saveGPSTimerInfo()
{
   int ret;
   int curYear, curMonth, curDay, curHour, curMinute, curSecond;
  Serial.print("save time\r\n");
  while((ret = gpsScan()) == -1);;
  getGpsTime(&curYear, &curMonth, &curDay, &curHour, &curMinute, &curSecond);

  sdWriteMovingTime(lastYear, lastMonth, lastDay, lastHour, lastMinute, lastSecond,
                  curYear, curMonth, curDay, curHour, curMinute, curSecond);
}

void updateStartTime()
{
   int ret;
   int curYear, curMonth, curDay, curHour, curMinute, curSecond;

  Serial.print("update start time\r\n");
  while((ret = gpsScan()) == -1);;
  getGpsTime(&curYear, &curMonth, &curDay, &curHour, &curMinute, &curSecond);
  lastYear = curYear;
  lastMonth = curMonth;
  lastDay = curDay;
  lastHour = curHour;
  lastMinute = curMinute;
  lastSecond = curSecond;
}

void saveGPSInfo(void)
{
  int ret;
  float gpsSpeed;
  float alMeters;

  Serial.print("try to get gps data\r\n");
  while((ret = gpsScan()) == -1);
  Serial.print("scan done\r\n");
  
  getGpsSpeed(&gpsSpeed);
  getGpsAltitude(&alMeters);
  
  sdWriteSpeed(gpsSpeed);
  sdWriteAltitude(alMeters);
}

static void updateBatteryStatus(int BATVoltage)
{
  if (BATVoltage > BAT_POWER_FULL) {
    batteryPowerStatus =  full;
  } else if (BATVoltage > BAT_POWER_PREWARNNING
             && BATVoltage <= BAT_POWER_FULL) {
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
  BLEsetup();
  AppDebug("Initializing is Done.");
}

int temp;
void loop() {
  
    // for Debug
    batteryPowerStatus =  full;
    
  currentBATVoltage = analogRead(A1);
  updateBatteryStatus(currentBATVoltage);
  lastBATVoltage = currentBATVoltage;

    // for Debug
    batteryPowerStatus =  full;
    
  // 防止AD采样精度不准 增加余量
  if (currentBATVoltage > (lastBATVoltage - 100)) {
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
    chargingTimer = TIMER_0S;
  }
 
  // dataSample Button checking
  sampleButtonVoltage = analogRead(A2);
  if (sampleButtonVoltage > SAMPLE_BUTTON_PUSHED_VOLTAGE) {
    sampleButtonPushedTimer++;
    if (sampleButtonPushedTimer >= TIMER_1S) {
      if (dataSampleStart == true) {
        // 在采样时如果按钮按下大于1s，采样结束,读取GPS的值作为结束时间，并保持到SD卡中
        Serial.print("Stop Sample and Save Date\r\n");
        saveGPSTimerInfo();
        sdWriteDistance(distance);
        dataSampleStart = false;
        sampleButtonPushedTimer = 0;
      }
    }

    if (sampleButtonPushedTimer >= TIMER_1_5S) {
      // 按钮按下大于1.5s数据采样采样开始
      // 读取GPS时间值，作为运动起始时间
      Serial.print("update start Time\r\n");
      updateStartTime();
      updateGpslngAndlatValue();
      distance = 0;
      
      dataSampleStart = true;
      sampleButtonPushedTimer = 0;
    }
  } else {
    sampleButtonPushedTimer = 0;
  }

  // Debug Propose
  dataSampleStart = false;
  /*
  temp++;

  if (temp == 3) {
      updateStartTime();
      updateGpslngAndlatValue();
      distance = 0;     
  }
  
  if (temp == 50) {
        Serial.print("save GPS time info and distance");
        temp = 0;
        saveGPSTimerInfo();
        sdWriteDistance(distance);      
  }
 */
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
      
      getGpsDistance(&distanceAvrSec);
      distance += distanceAvrSec;
      
      saveGPSInfo();
      
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
  
  if (BLEisConnectted() >= 0) {
    // BLE is connected and send data to App by BLE
    Serial.print("Start sending BLE data");
    BLESending = true;
    CurieTimerOne.start(BLE_LEDS_INTERVAL_TIMERS, &timedBlinkIsr);  // set timer and callback
    BLEupdate();
    CurieTimerOne.kill();
  } else {
    BLESending = false;
  }

  if (!(BATCharging || DataSampling || BLESending)) {
    LedsShowBATStatus(batteryPowerStatus);
  }

  delay(LOOP_INTERVAL);
}

