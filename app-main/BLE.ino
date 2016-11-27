#include <BLEAttribute.h>
#include <BLECentral.h>
#include <BLECharacteristic.h>
#include <BLECommon.h>
#include <BLEDescriptor.h>
#include <BLEPeripheral.h>
#include <BLEService.h>
#include <BLETypedCharacteristic.h>
#include <BLETypedCharacteristics.h>
#include <BLEUuid.h>
#include <CurieBle.h>
#include <SD.h>

#include "SdCard.h"

BLEPeripheral blePeripheral;

BLEService HeartRateService("1234aa10-5678-9abc-def0-123456789abc");
BLECharacteristic HeartRateResponseCharacteristic("1234aa12-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);
BLECharacteristic HeartRateCommandCharacteristic("1234aa11-5678-9abc-def0-123456789abc",
    BLEWrite, 1);

BLEService DistanceService("1234aa20-5678-9abc-def0-123456789abc");
BLECharacteristic DistanceCommandCharacteristic("1234aa21-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic DistanceResponseCharacteristic("1234aa22-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

BLEService SpeedRateService("1234aa30-5678-9abc-def0-123456789abc");
BLECharacteristic SpeedRateCommandCharacteristic("1234aa31-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic SpeedRateResponseCharacteristic("1234aa32-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

BLEService AltitudeService("1234aa40-5678-9abc-def0-123456789abc");
BLECharacteristic AltitudeCommandCharacteristic("1234aa41-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic AltitudeResponseCharacteristic("1234aa42-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

BLEService StepNumberService("1234aa50-5678-9abc-def0-123456789abc");
BLECharacteristic StepNumberCommandCharacteristic("1234aa51-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic StepNumberResponseCharacteristic("1234aa52-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

BLEService BodyFatRateService("1234aa60-5678-9abc-def0-123456789abc");
BLECharacteristic BodyFatRateCommandCharacteristic("1234aa61-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic BodyFatRateResponseCharacteristic("1234aa62-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

BLEService TimeService("1234aa70-5678-9abc-def0-123456789abc");
BLECharacteristic TimeCommandCharacteristic("1234aa71-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic TimeResponseCharacteristic("1234aa72-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

/*BLEService HardwareVersionService("1234aa80-5678-9abc-def0-123456789abc");
  BLECharacteristic HardwareVersionCommandCharacteristic("1234aa81-5678-9abc-def0-123456789abc",
                  BLEWrite, 1);
  BLECharacteristic HardwareVersionResponseCharacteristic("1234aa82-5678-9abc-def0-123456789abc",
                  BLERead | BLENotify, 20); */                                                 //备注：因为硬件版本不是特别重要的参数，鉴于只能有8个service，暂时不用传输该信息；APP对应的Service的UUID需要体会为DateService中的UUID，图标需要替换；

BLEService DateService("1234aa90-5678-9abc-def0-123456789abc");
BLECharacteristic DateCommandCharacteristic("1234aa91-5678-9abc-def0-123456789abc",
    BLEWrite, 1);
BLECharacteristic DateResponseCharacteristic("1234aa92-5678-9abc-def0-123456789abc",
    BLERead | BLENotify, 20);

/* last BodyFatRate the connect state was checked, in ms */

unsigned char command[1] = {0x00};
bool HeartRateReadOrNot = false;

void BLEsetup() {
  //Serial.begin(9600);
  blePeripheral.setLocalName("BLE_Demo");
  blePeripheral.setAdvertisedServiceUuid(DateService.uuid());                                 // add the service UUID 备注：因为数据是以日期来分类管理的，所以以时间的UUID来进行辨识，比心率的辨识更好一些；APP需要修改相应的UUID连接；

  blePeripheral.addAttribute(HeartRateService);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(HeartRateResponseCharacteristic); // add the Heart Rate Measurement characteristic
  blePeripheral.addAttribute(HeartRateCommandCharacteristic);

  blePeripheral.addAttribute(DistanceService);
  blePeripheral.addAttribute(DistanceCommandCharacteristic);
  blePeripheral.addAttribute(DistanceResponseCharacteristic);

  blePeripheral.addAttribute(SpeedRateService);
  blePeripheral.addAttribute(SpeedRateCommandCharacteristic);
  blePeripheral.addAttribute(SpeedRateResponseCharacteristic);

  blePeripheral.addAttribute(AltitudeService);
  blePeripheral.addAttribute(AltitudeCommandCharacteristic);
  blePeripheral.addAttribute(AltitudeResponseCharacteristic);

  blePeripheral.addAttribute(StepNumberService);
  blePeripheral.addAttribute(StepNumberCommandCharacteristic);
  blePeripheral.addAttribute(StepNumberResponseCharacteristic);

  blePeripheral.addAttribute(BodyFatRateService);
  blePeripheral.addAttribute(BodyFatRateCommandCharacteristic);
  blePeripheral.addAttribute(BodyFatRateResponseCharacteristic);

  blePeripheral.addAttribute(TimeService);                                                         //备注：此处的time，是指运动的时间起始、截止时间、时间长度；不是当前时间；APP中的标识图标是否需要修改；
  blePeripheral.addAttribute(TimeCommandCharacteristic);
  blePeripheral.addAttribute(TimeResponseCharacteristic);

  /*  blePeripheral.addAttribute(HardwareVersionService);
    blePeripheral.addAttribute(HardwareVersionCommandCharacteristic);
    blePeripheral.addAttribute(HardwareVersionResponseCharacteristic); */                             //备注：因为硬件版本不是特别重要的参数，鉴于只能有8个service，暂时不用传输该信息；APP中对应的UUID与标识需要修改；

  blePeripheral.addAttribute(DateService);
  blePeripheral.addAttribute(DateCommandCharacteristic);
  blePeripheral.addAttribute(DateResponseCharacteristic);

  /* Now activate the BLE device.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  blePeripheral.begin();
}

int BLEisConnectted()
{
  int ret;
  BLECentral central = blePeripheral.central();
  if (central) {
    ret = central.connected();
    Serial.print(ret);
    return ret;
  }
  else
    return -1;
}

void BLEupdate() {
  BLECentral central = blePeripheral.central();
  if (central) {
    while (central.connected()) {
      if (HeartRateCommandCharacteristic.written()) {
        memcpy(command, HeartRateCommandCharacteristic.value(), 2);
        if (command[0] == 0x00) {}
        else if (command[0] == 0x01) {
          //updateHeartRate(500);
          updateHeartRate();
        }
      }

      else if (DistanceCommandCharacteristic.written()) {
        memcpy(command, DistanceCommandCharacteristic.value(), 2);
        if (command[0] == 0x02) {
          //updateDistance(1);
          updateDistance();
        } else if (command[0] == 0x00) {}
      }

      else if (SpeedRateCommandCharacteristic.written()) {
        memcpy(command, SpeedRateCommandCharacteristic.value(), 2);
        if (command[0] == 0x03) {
          //updateSpeedRate(500);
          updateSpeedRate();
        } else if (command[0] == 0x00) {}
      }

      else if (AltitudeCommandCharacteristic.written()) {
        memcpy(command, AltitudeCommandCharacteristic.value(), 2);
        if (command[0] == 0x04) {
          //updateAltitude(200);
          updateAltitude();
        } else if (command[0] == 0x00) {}
      }

      else if (StepNumberCommandCharacteristic.written()) {
        memcpy(command, StepNumberCommandCharacteristic.value(), 2);
        if (command[0] == 0x05) {
          updateStepNumber();
        } else if (command[0] == 0x00) {}
      }
      /*
        if (command[0] == 0x05) {updateStepNumber(1);}else if (command[0] == 0x00) {}
        }
      */
      else if (BodyFatRateCommandCharacteristic.written()) {
        memcpy(command, BodyFatRateCommandCharacteristic.value(), 2);
        if (command[0] == 0x06) {
          updateBodyFatRate(1);
        } else if (command[0] == 0x00) {}
      }

      else if (TimeCommandCharacteristic.written()) {         //备注：Time是指运动的起点、结束点、时间长度，不是当前时间，只需要读取3个值就可以了，显示3个数据；1）APP显示方式从图形显示改为数字显示；２）ＡＰＰ接受到数据后的存储需要按照日期（updateDate(3)所获得数据）、时间（updateTime(3)所获得数据）来保存；
        memcpy(command, TimeCommandCharacteristic.value(), 2);
        if (command[0] == 0x07) {
          updateTime(1);
        } else if (command[0] == 0x00) {}
      }

      /*    else if (HardwareVersionCommandCharacteristic.written()) {
            memcpy(command, HardwareVersionCommandCharacteristic.value(), 2);
            if (command[0] == 0x08) {updateHardwareVersion();}else if (command[0] == 0x00) {}
           } */                                                                                     //备注：因为硬件版本不是特别重要的参数，鉴于只能有8个service，暂时不用传输该信息；

      else if (DateCommandCharacteristic.written()) {
        memcpy(command, DateCommandCharacteristic.value(), 1);
        if (command[0] == 0x09) {
          updateDate(1);
        } else if (command[0] == 0x00) {}
      }
    }
  }
}

void createBLEFrame(int cmd, int len, char *orgData, unsigned char *response)
{
  int i;
  memset(response, 0, 20);
  response[0] = 0x7E; /* head */
  response[1] = 0x11; /* version */
  response[2] = cmd;    /* command */
  response[3] = len;  /* length  每帧的数据长度*/
  for (i = 0; i < len; i++) {
    response[4 + i] = orgData[i];
  }

  response[17] = crc8((uint8_t *)response, 16); /* crc8 */
  response[18] = 0x0a; /* end */
  response[19] = 0x0d; /* end */
}

//void updateHeartRate(unsigned int L)       //L为整体数据长度；                                    //备注：要求APP读取该数据串，读取完成后，停止按钮才能停止；数据格式：XXX（次/分）；
void updateHeartRate(void)
{
  const int HeartRateCmd = 1;
  int index = 0;
  unsigned char response[20] = {0};
  char frameBuffer[13] = {0};
  char inputChar;

  File fd = SD.open(heartRateFile, FILE_READ);
  while ((inputChar = fd.read()) != EOF) {
    Serial.print(inputChar);
    if (inputChar != '\r' || inputChar != '\n') {
      frameBuffer[index++] = inputChar;
      if (index == 13) {
        createBLEFrame(HeartRateCmd, index, frameBuffer, response);
        HeartRateResponseCharacteristic.setValue(response, 20);
        memset(frameBuffer, 0, 13);
        index = 0;
      }
    }
  }

  if (index != 0) {
    createBLEFrame(HeartRateCmd, index, frameBuffer, response);
    HeartRateResponseCharacteristic.setValue(response, 20);
  }
}

//void updateDistance(unsigned int L)                                                                             //备注：APP只读取1个数据；只显示1个数据；数据格式：XXXX.XX（千米）；
void updateDistance(void)       
{
  const int DistanceCmd = 2;
  int index = 0;
  unsigned char response[20] = {0};
  char frameBuffer[13] = {0};
  char inputChar;

  File fd = SD.open(distanceFile, FILE_READ);
  while ((inputChar = fd.read()) != EOF) {
    Serial.print(inputChar);
    if (inputChar != '\r' || inputChar != '\n') {
      frameBuffer[index++] = inputChar;
      if (index == 13) {
        createBLEFrame(DistanceCmd, index, frameBuffer, response);
        DistanceResponseCharacteristic.setValue(response, 20);
        memset(frameBuffer, 0, 13);
        index = 0;
      }
    }
  }

  if (index != 0) {
    createBLEFrame(DistanceCmd, index, frameBuffer, response);
    DistanceResponseCharacteristic.setValue(response, 20);
  }
}


//void updateSpeedRate(unsigned int L)                                                                       //备注：数据格式：XX.X(米/秒);
void updateSpeedRate(void)  
{
  const int SpeedCmd = 3;
  int index = 0;
  unsigned char response[20] = {0};
  char frameBuffer[13] = {0};
  char inputChar;

  File fd = SD.open(speedFile, FILE_READ);
  while ((inputChar = fd.read()) != EOF) {
    Serial.print(inputChar);
    if (inputChar != '\r' || inputChar != '\n') {
      frameBuffer[index++] = inputChar;
      if (index == 13) {
        createBLEFrame(SpeedCmd, index, frameBuffer, response);
        SpeedRateResponseCharacteristic.setValue(response, 20);
        memset(frameBuffer, 0, 13);
        index = 0;
      }
    }
  }

  if (index != 0) {
    createBLEFrame(SpeedCmd, index, frameBuffer, response);
    SpeedRateResponseCharacteristic.setValue(response, 20);
  }
}

//void updateAltitude(unsigned int L)                                                                        //备注：数据格式：XXXX（米）；
void updateAltitude(void) 
{
  const int AltitudeCmd = 4;
  int index = 0;
  unsigned char response[20] = {0};
  char frameBuffer[13] = {0};
  char inputChar;

  File fd = SD.open(speedFile, FILE_READ);
  while ((inputChar = fd.read()) != EOF) {
    Serial.print(inputChar);
    if (inputChar != '\r' || inputChar != '\n') {
      frameBuffer[index++] = inputChar;
      if (index == 13) {
        createBLEFrame(AltitudeCmd, index, frameBuffer, response);
        AltitudeResponseCharacteristic.setValue(response, 20);
        memset(frameBuffer, 0, 13);
        index = 0;
      }
    }
  }

  if (index != 0) {
    createBLEFrame(AltitudeCmd, index, frameBuffer, response);
    AltitudeResponseCharacteristic.setValue(response, 20);
  }
}

//void updateStepNumber(unsigned int L)                                                               //备注：APP只读取1个数据；只显示1个数据；数据格式：XXXXX（步）
void updateStepNumber(void)
{
  const int StepNumberCmd = 5;
  int index = 0;
  unsigned char response[20] = {0};
  char frameBuffer[13] = {0};
  char inputChar;

  File fd = SD.open(stepCoutFile, FILE_READ);
  while ((inputChar = fd.read()) != EOF) {
    Serial.print(inputChar);
    if (inputChar != '\r' || inputChar != '\n') {
      frameBuffer[index++] = inputChar;
      if (index == 13) {
        createBLEFrame(StepNumberCmd, index, frameBuffer, response);
        StepNumberResponseCharacteristic.setValue(response, 20);
        memset(frameBuffer, 0, 13);
        index = 0;
      }
    }
  }

  if (index != 0) {
    createBLEFrame(StepNumberCmd, index, frameBuffer, response);
    StepNumberResponseCharacteristic.setValue(response, 20);
  }
}

void updateBodyFatRate(unsigned int L)                                                             //备注：APP只读取1个数据；只显示1个数据；数据格式：XX.X%；
{
  unsigned char response[20] = {0};
  unsigned int i = 0;
  unsigned char j = 0;
  unsigned int len = 0;
  for (i = 0; i < L; i = i + 13)
  { len = (L - i) >= 13 ? 13 : L - i;
    response[0] = 0x7E; /* head */
    response[1] = 0x11; /* version */
    response[2] = 6;    /* command */
    response[3] = len;  /* length  每帧的数据长度*/
    for (j = 4; j < 17; j++)
    { response[j] = random(10, 100);
    }
    response[17] = crc8((uint8_t *)response, 16); /* crc8 */
    response[18] = 0x0a; /* end */
    response[19] = 0x0d; /* end */
    BodyFatRateResponseCharacteristic.setValue(response, 20);
  }
}

void updateTime(unsigned int L)                                                                 //备注：此函数为运动的时间，APP只读取1个数据；只显示1个数据；数据格式：起始时刻：XX.XX（前面XX为1-24，小数点后的XX为0-60）、截止时刻：格式与起始时刻表示方法相同；时间长度：XX.XX与起始时刻表示方法相同；
{
  unsigned char response[20] = {0};
  unsigned int i = 0;
  unsigned char j = 0;
  unsigned int len = 0;
  for (i = 0; i < L; i = i + 13)
  { len = (L - i) >= 13 ? 13 : L - i;
    response[0] = 0x7E; /* head */
    response[1] = 0x11; /* version */
    response[2] = 7;    /* command */
    response[3] = len;  /* length  每帧的数据长度*/
    for (j = 4; j < 17; j++)
    { response[j] = random(10, 100);
    }
    response[17] = crc8((uint8_t *)response, 16); /* crc8 */
    response[18] = 0x0a; /* end */
    response[19] = 0x0d; /* end */
    TimeResponseCharacteristic.setValue(response, 20);
  }
}

/*void updateHardwareVersion()
  {
  unsigned char response[20] = {0};
  response[0] = 0x7E; /* head */
/*  response[1] = 0x11; /* version */
/*  response[2] = 8;    /* command */
/*  response[3] = 4;    /* length */
/*  response[4] = random(10,200);   /* suppose 99 now */
/*  response[5] = crc8((uint8_t *)response, 5); /* crc8 */
/*  response[6] = 0x0a; /* end */
/*  response[7] = 0x0d; /* end */
/*  HardwareVersionResponseCharacteristic.setValue(response, 20);
  }    */

void updateDate(unsigned int L)                            //备注：1)此函数为运动的日期，APP只读取1次数据；只显示日期；2)日期的格式将为XXXXXXXX，其中前四位XXXX为年，中间两位XX为月；后面两位为XX日；APP中请按照这种格式解码；
{
  unsigned char response[20] = {0};
  unsigned int i = 0;
  unsigned char j = 0;
  unsigned int len = 0;
  for (i = 0; i < L; i = i + 13)
  { len = (L - i) >= 13 ? 13 : L - i;
    response[0] = 0x7E; /* head */
    response[1] = 0x11; /* version */
    response[2] = 9;    /* command */
    response[3] = len;  /* length  每帧的数据长度*/
    for (j = 4; j < 17; j++)
    { response[j] = random(10, 100);
    }
    response[17] = crc8((uint8_t *)response, 16); /* crc8 */
    response[18] = 0x0a; /* end */
    response[19] = 0x0d; /* end */
    DateResponseCharacteristic.setValue(response, 20);
  }
}

uint8_t crc8(uint8_t *addr, uint8_t len)
{
  uint8_t crc = 0;

  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

