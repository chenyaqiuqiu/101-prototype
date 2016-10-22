#include <SPI.h>
#include <SD.h>

File myFile;
// file name length is limitted
const char *heartBeatFile = "HB.txt";
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
    Serial.println("initialization failed!");
  }
}

//O_APPEND

void sdWriteHeartBeat(int val)
{
  char result[3] = {0};
  sprintf(result, "%03d", val);
  myFile = SD.open(heartBeatFile, FILE_WRITE);
  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  Serial.println("Write HeartBeat done.");
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
  
  Serial.println("Write StepCout done.");
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
  Serial.println("Write Acl And Gyro done.");
}

void  sdWriteDistance(int distance)
{
  char result[4] = {0};
  sprintf(result, "%04d", distance);
  myFile = SD.open(distanceFile, FILE_WRITE);
  myFile.seek(0);
  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  
  Serial.println("Write Distance done.");  
}

void sdWriteSpeed(float speedKm)
{
  char speedValue[4] = {0};
  sprintf(speedValue, "%04.1f", speedKm);

  myFile = SD.open(speedFile, FILE_WRITE);
  myFile.print(speedValue);
    // close the file:
  myFile.close();
  myFile.flush();

  Serial.println("Write Speed done.");  
}

void  sdWriteAltitude(int alMeters)
{
  char result[4] = {0};
  sprintf(result, "%04d", alMeters);
  myFile = SD.open(AltitudeFile, FILE_WRITE);
  myFile.print(result);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();  
}

void SdFileRemove(const char *file)
{
  SD.remove(file);
}

