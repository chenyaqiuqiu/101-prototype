/*
 *  SD operation for Arduino 101
 */
 
#ifndef SD_H
#define SD_H

void sdCardSetup(void);
void sdWriteHeartBeat(long heartBeat);
void sdWriteStepCount(long stepcount);

#endif
