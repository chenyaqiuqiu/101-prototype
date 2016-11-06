#include <SPI.h>
#include <SD.h>

#define SD_DEBUG 1

#ifdef SD_DEBUG
#define SdDebug(x) Serial.println(x)
#else
#define SdDebug(x) {}
#endif

File myFile;
// file name length is limitted
const char *heartRateFile = "HB.txt";
const char *stepCoutFile = "steps.txt";
const char *aclAndGyroFile = "AG.txt";
const char *gpsFile = "gps.txt";

const char *distanceFile = "DST.txt";
const char *speedFile = "speed.txt";
const char *AltitudeFile = "meters.txt";
const char *dateFile = "date.txt";

void sdCardSetup(int csPin)
{
  if (!SD.begin(csPin)) {
    SdDebug("SD card init failed!\r\n");
  }
}

//O_APPEND

void sdWriteHeartRate(int val)
{
  char result[3] = {0};
  sprintf(result, "%03d", val);
  myFile = SD.open(heartRateFile, FILE_WRITE);
  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  SdDebug("Write heartRate done.");
}

void sdWriteStepCount(long stepcount)
{
  char result[5] = {0};
  sprintf(result, "%05d", stepcount);
  myFile = SD.open(stepCoutFile, FILE_WRITE);
  myFile.seek(0);
  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();

  SdDebug("Write StepCout done.");
}

void sdWriteAclAndGyro(float gx, float gy, float gz, float aclx, float acly, float aclz)
{
  char gValue[6] = {0};
  char aclValue[6] = {0};

  myFile = SD.open(aclAndGyroFile, FILE_WRITE);
  myFile.print("//");
  sprintf(gValue, "%06.2f", gx);
  myFile.print(gValue);
  myFile.print("//");
  sprintf(gValue, "%06.2f", gy);
  myFile.print(gValue);
  myFile.print("//");
  sprintf(gValue, "%06.2f", gz);
  myFile.print(gValue);
  myFile.print("//");
  sprintf(aclValue, "%06.3f", aclx);
  myFile.print(aclValue);
  myFile.print("//");
  sprintf(aclValue, "%06.3f", acly);
  myFile.print(aclValue);
  myFile.print("//");
  sprintf(aclValue, "%06.3f", aclz);
  myFile.print(aclValue);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  SdDebug("Write Acl And Gyro done.");
}

void  sdWriteDistance(int distance)
{
  int i;
  char result[5] = {0};

  sprintf(result, "%05d", distance);
  myFile = SD.open(distanceFile, FILE_WRITE);
  myFile.seek(0);

  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();

  SdDebug("Write Distance done.");
}

void sdWriteSpeed(float speedKm)
{
  int i;
  char speedValue[4] = {0};
 
  sprintf(speedValue, "%04.1f", speedKm);
  myFile = SD.open(speedFile, FILE_WRITE);
  for (i = 0; i < 4; i++) {
    myFile.print(speedValue[i]);
  }
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();

  SdDebug("Write Speed done.");
}

void  sdWriteAltitude(float alMeters)
{
  int i;
  char result[4] = {0};

  sprintf(result, "%04d", (int)alMeters);
  myFile = SD.open(AltitudeFile, FILE_WRITE);

  for (i = 0; i < 4; i++) {
    myFile.print(result[i]);
  }

  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
}

int sdReadLine(char *dataBuffer)
{
  char inputChar;
  int index = 0;
  //myFile = SD.open(file, FILE_READ);
  inputChar = myFile.read();
  Serial.print(inputChar);
  if (inputChar == EOF)
    return  -1;

  while (inputChar != '\n')
  {
    dataBuffer[index++] = inputChar;
    inputChar = myFile.read();
    if (inputChar == EOF)
      return 0;
  }
  return index;
}

void sdReadheartRateFile(void)
{
  char oneLine[12] = {0};
  int ret;
  myFile = SD.open(heartRateFile, FILE_READ);

  while ((ret = sdReadLine(oneLine)) > 0) {
    Serial.print(oneLine);
    memset(oneLine, 0, 12);
  }

  myFile.close();
}

void SdFileRemove(const char *file)
{
  SD.remove(file);
}

