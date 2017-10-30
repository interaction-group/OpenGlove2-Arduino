#include "Arduino.h"

boolean get_calibrationStatus();
void set_threshold();
void set_Threshold(int value);
boolean checkFlexor(int flexors[], int pin);
int readSensor(int sensorValue,int r_min,int r_max);
int simpleValue(int value);
int validValue( int flexors[], int value, int indice);
void calibrateFlexors(int flexors[]);
void addFlexor(int flexors[]);
void removeFlexor(int flexors[]);
void resetFlexors(int flexors[]);
