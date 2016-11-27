/*
 *  SD operation for Arduino 101
 */
 
#ifndef SD_H
#define SD_H

// file name length is limitted
const char *heartRateFile = "HB.TXT";
//const char *stepCoutFile = "steps.txt";
const char *stepCoutFile = "STEPS.TXT";
const char *aclAndGyroFile = "AG.txt";
const char *gpsFile = "gps.txt";

const char *distanceFile = "DST.txt";
const char *speedFile = "speed.txt";
const char *AltitudeFile = "meters.txt";
const char *dateFile = "date.txt";


void sdCardSetup(int csPin);
void sdWriteHeartRate(int val);
void sdWriteAclAndGyro(float gx, float gy, float gz, float aclx, float acly, float aclz);

void sdWriteDistance(int distance);
void sdWriteSpeed(float speedKm);
void sdWriteAltitude(int alMeters);
void sdWriteMovingTime(int Lyear, int Lmonth, int Lday, int Lhour, int Lminute, int Lsecond,
                  int Cyear, int Cmonth, int Cday, int Chour, int Cminute, int Csecond);
                  
void sdReadheartRateFile(void);
int sdReadLine(const char *path,  char *dataBuffer);
#endif
