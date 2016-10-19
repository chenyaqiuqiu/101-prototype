#include <SPI.h>
#include <SD.h>

File myFile;
// file name length is limitted
const char *heartBeatFile = "HB.txt";
const char *stepCoutFile = "SC.txt";
const char *aclAndGyroFile = "AG.txt";
const char *gpsFile = "gps.txt";

void sdCardSetup(int csPin)
{
  if (!SD.begin(csPin)) {
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

void sdWriteAclAndGyro(float gx, float gy, float gz, float aclx, float acly, float aclz)
{
  myFile = SD.open(aclAndGyroFile, FILE_WRITE);
  myFile.print("//");
  myFile.print(gx);
  myFile.print("//");
  myFile.print(gy);
  myFile.print("//");
  myFile.print(gz);
  myFile.print("//");
  myFile.print(aclx);
  myFile.print("//");
  myFile.print(acly);
  myFile.print("//");
  myFile.print(aclz);
  myFile.print("\r\n");
  // close the file:
  myFile.close();
  myFile.flush();
  Serial.println("Write Acl And Gyro done.");
}

void SdFileRemove(const char *file)
{
  SD.remove(file);
}

