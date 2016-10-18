#include <SPI.h>
#include <SD.h>

File myFile;
// file name length is limitted
const char *heartBeatFile = "HB.txt";
const char *stepCoutFile = "SC.txt";
const char *aclAndGyroFile = "AG.txt";
const char *gpsFile = "gps.txt";

void sdCardSetup(void)
{
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
  }
}

//O_APPEND

void sdWriteHeartBeat(int heartBeat)
{
  myFile = SD.open(heartBeatFile, FILE_WRITE);
  myFile.print(heartBeat);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  Serial.println("Write HeartBeat done.");
}

void sdWriteStepCount(long stepcount)
{
  myFile = SD.open(stepCoutFile, FILE_WRITE);
  myFile.seek(0);
  myFile.print(stepcount);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  Serial.println("Write StepCout done.");
}

void writeAclAndGyro(int heartBeat)
{
  myFile = SD.open(aclAndGyroFile, FILE_WRITE);
  myFile.print(heartBeat);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  Serial.println("Write HeartBeat done.");
}

void SdFileRemove(const char *file)
{
  SD.remove(file);
}

