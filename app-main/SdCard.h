/*
 *  SD operation for Arduino 101
 */
 
#ifndef SD_H
#define SD_H

void sdCardSetup(int csPin);
void sdWriteHeartBeat(long heartBeat);
void sdWriteAclAndGyro(float gx, float gy, float gz, float aclx, float acly, float aclz);

#endif
