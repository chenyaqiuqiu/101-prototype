/*
 *  GYRO header File
 */

#ifndef GPS_H
#define GPS_H

void gpsSetup(int gpsBaud);
//int getGpshdopAndsateNum(int *satellitesNum, int *hdop);
//int getGpslngAndlatValue(double *lat, double *lng, int *age);

int updateGpslngAndlatValue(void);
void getGpsDistance(int *distance);
int getGpsSpeed(float *speed);
int getGpsAltitude(float *altitudeMeters);
int getGpsTime(int *year, int *month, int *day, int *hour, int *minute, int *second);
void smartDelay(unsigned long ms);
int gpsScan(void);

#endif
