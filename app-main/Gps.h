/*
 *  GYRO header File
 */

#ifndef GPS_H
#define GPS_H

void gpsSetup(int gpsBaud);
//int getGpshdopAndsateNum(int *satellitesNum, int *hdop);
//int getGpslngAndlatValue(double *lat, double *lng, int *age);

void getGpsDistance(int *distance);
int getGpsSpeed(float *speed);
int getGpsAltitude(int *altitudeMeters);
int getGpsTime(int *year, int *month, int *day, int *hour, int *minute, int *second);

#endif
