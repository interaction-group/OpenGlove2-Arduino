#include "Arduino.h"
#include "FlexorsFunctions.h"

static char TERMINAL_SIGN = 's';

static const int TotalFlexors = 10;
int flexors_lastValue[TotalFlexors];
int flexors_min[TotalFlexors];
int flexors_max[TotalFlexors];
int readValue;
int calibration_time = 3000;
boolean calibrate = false;

boolean threshold_On = true;
int threshold = 0;

void addFlexor(int flexors[]){
  int pin = Serial.parseInt();
  int mapping = Serial.parseInt();
  
  if(Serial.read() == TERMINAL_SIGN){
    //COMENTANDO ESTO PERMITE AÑADIR UN FLEXOR EN MÁS DE UNA POSICION
    /*for(int i=0;i<TotalFlexors;i++){
      if(flexors[i]==pin){
        return;  
      }
    }*/
    pinMode(pin,INPUT);
    flexors[mapping]=pin;
    if(calibrate==true){
      int min_avg = 0;
      int max_avg = 0;
      int count = 0;
      if(mapping<5){
        for(int i=0; i<5;i++){
          if(flexors[i]>-1 && i!=mapping){
            count++;
            min_avg=min_avg+flexors_min[i];
            max_avg=max_avg+flexors_max[i];
          }
        }
      }else{
        for(int i=5; i<TotalFlexors;i++){
          if(flexors[i]>-1 && i!=mapping){
            count++;
            min_avg=min_avg+flexors_min[i];
            max_avg=max_avg+flexors_max[i];
          }
        } 
      }
      if(count>0){
        min_avg=(min_avg/count);
        max_avg=(max_avg/count);
        flexors_min[mapping]=min_avg;
        flexors_max[mapping]=max_avg;
        flexors_lastValue[mapping]=-1;
      }else{
        //implementar calibración individual
        flexors_min[mapping]=80;
        flexors_max[mapping]=210;
        flexors_lastValue[mapping]=-1;
      }
    }else{
      flexors_min[mapping]=-1;
      flexors_max[mapping]=-1;
      flexors_lastValue[mapping]=-1;
    } 
  }
  
}

void removeFlexor(int flexors[]){
  int mapping = Serial.parseInt();
  if(Serial.read() == TERMINAL_SIGN){
    if(flexors[mapping]>-1){
      flexors[mapping]=-1;
      flexors_lastValue[mapping]=-1;
      flexors_min[mapping]=-1;
      flexors_max[mapping]=-1;
    }  
  }
}

boolean get_calibrationStatus(){
  return calibrate;
}

void set_threshold(){
  readValue = Serial.parseInt();
  if(Serial.read() == TERMINAL_SIGN){
    threshold=readValue;  
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
  if(value<50 || value>350){
    return -1;
  }
  return value;
}

int validValue( int flexors[], int value, int indice){
  if(value<50 || value>350){
    return -1;
  }
  else{
    if(calibrate==true){
      value = readSensor(value,flexors_min[indice],flexors_max[indice]);
    }else{
      value = readSensor(value,80,210);
    }
    if(threshold_On == true){
      if(flexors_lastValue[indice]== -1){
        flexors_lastValue[indice]=value;
        return value;
      }
      if((value <= flexors_lastValue[indice]-threshold ) || (value >= flexors_lastValue[indice]+threshold ) || value == 0 || value == 100 ){
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
void calibrateFlexors(int flexors[])
{
  int value;
  boolean valid = false;
  if(Serial.read() == TERMINAL_SIGN){
  
    delay(2000);
    for(int i =0;i < TotalFlexors; i++){
      if(flexors[i]>-1){
        value=analogRead(flexors[i]);
        while(simpleValue(value) == -1){
            value=analogRead(flexors[i]);
        }
         flexors_min[i]=value;
         flexors_max[i]=value;
         flexors_lastValue[i]=-1;
      }
    }
    
    int exit_f=0;
    //Serial.println("Calibrando, por favor abra y cierre la mano. Presione 1 para finalizar");
    while(exit_f!='e'){
      
        for(int i=0; i<TotalFlexors;i++){
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
        if(Serial.available() > 0){
          exit_f = Serial.read();
        }
        delay(100);
      }
      calibrate=true;
  }
  
}

void resetFlexors(int flexors[]){
  if(Serial.read() == TERMINAL_SIGN){
      for(int i=0;i<TotalFlexors;i++){
      flexors[i]=-1;
      flexors_min[i]=-1;
      flexors_max[i]=-1;
      flexors_lastValue[i]=-1;
    }
    threshold=0;
    calibrate=false;     
  }
}

