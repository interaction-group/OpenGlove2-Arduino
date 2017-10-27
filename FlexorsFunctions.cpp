#include "Arduino.h"
#include "FlexorsFunctions.h"

static const int TotalFlexors = 10;
int flexors_lastValue[TotalFlexors];
int flexors_min[TotalFlexors];
int flexors_max[TotalFlexors];
int readValue;
int calibration_time = 3000;
boolean calibrate = false;

boolean threshold_On = true;
int threshold = 5;

boolean get_calibrationStatus(){
  return calibrate;  
}

boolean set_threshold(){
  readValue = Serial.parseInt();
  threshold=readValue;  
}

void set_Threshold(int value){
    if(value>=0 && value<100){
        threshold=value;
      }
}

boolean checkFlexor(int flexors[], int pin){
  for(int i=0; i < TotalFlexors; i++){
    if(flexors[i]==pin){
      return true;
    }
  }
  return false;
}

//LEE EL SENSOR CALIBRADO
int readSensor(int sensorValue,int r_min,int r_max)
{
  int value = map(sensorValue, r_max, r_min,0, 100);
  if(value<=0){
    return 0;  
  }
  if(value>=100){
    return 100;
  }
  return value;
}

int simpleValue(int value){
  if(value<90 || value>280){
    return -1;
  }
  return value;
}

int validValue( int flexors[], int value, int indice){
  if(value<90 || value>280){
    return -1;
  }
  else{
    value = readSensor(value,flexors_min[indice],flexors_max[indice]);
    if(threshold_On == true){
      if(flexors_lastValue[indice]== -1){
        flexors_lastValue[indice]=value;
        return value;
      }
      if((value <= flexors_lastValue[indice]-threshold ) || (value >= flexors_lastValue[indice]+threshold )){
        flexors_lastValue[indice]=value;
        return value;
      }else{
        return -1;  
      }
    }
   else{
      return value;
    }
  }
}


// FUNCION PARA CALIBRAR LOS SENSORES
void calibrateFlexors(int flexors[], int count_sensors)
{
  int value;
  boolean valid = false;
  if(calibrate==false){
        delay(2000);
        for(int i =0;i < count_sensors; i++){
          if(flexors[i]>-1){
            value=analogRead(flexors[i]);
            while(simpleValue(value) == -1){
                value=analogRead(flexors[i]);
            }
             flexors_min[i]=value;
             flexors_max[i]=value;
          }
      }
  }
  int exit_f=0;
  //Serial.println("Calibrando, por favor abra y cierre la mano. Presione 1 para finalizar");
  while(exit_f!=1){
    if(Serial.available() > 0){
      exit_f = Serial.parseInt();
    }
      for(int i=0; i<count_sensors;i++){
          if(flexors[i]>-1){
             value=analogRead(flexors[i]);
             if(simpleValue(value)>-1){

                if(flexors_max[i]< value){
                  flexors_max[i]= value;
                }
                
                if(flexors_min[i]> value){
                  flexors_min[i]= value;
                }

             }
          }
      }
        delay(100);
    }
    calibrate=true;
}

