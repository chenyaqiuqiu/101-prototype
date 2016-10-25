/*
 *  SD operation for Arduino 101
 */
 
#ifndef SD_H
#define SD_H

void sdCardSetup(int csPin);
void sdWriteHeartRate(int val);
void sdWriteAclAndGyro(float gx, float gy, float gz, float aclx, float acly, float aclz);

void sdWriteDistance(int distance);
void sdWriteSpeed(float speedKm);
void sdWriteAltitude(int alMeters);

void sdReadheartRateFile(void);

#endif
