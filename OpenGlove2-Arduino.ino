static char TERMINAL_SIGN = 's';

int flexors[10];
int flexors_lastValue[10];
int flexors_min[10];
int flexors_max[10];

int function;

boolean calibrate;
boolean threshold_On;

int count_sensors;
int flex_value;
int threshold;

void setup() {
  Serial.begin(57600);
  for(int i =0;i < 10; i++){
      flexors[i]=-1;
  }
  flexors[0]=16;
  flexors[1]=17;
  flexors[2]=19;
  count_sensors=3;
  threshold=2;

  for(int i =0;i < count_sensors; i++){
    if(flexors[i]>-1){
      pinMode(flexors[i],INPUT);
      flexors_lastValue[i]=-1;
    }
  }
  calibrate=false;
  threshold_On=true;
}

void calcular_Max(int actual, int value){
    if(flexors_max[actual]< value){
        flexors_max[actual]= value;
       // Serial.println(String(flexors[actual])+"nuevo max: "+String(value));
      }
  }

void set_Threshold(int value){
    if(value>=0 && value<100){
        threshold=value;
      }
  }


void calcular_Min(int actual, int value){
    if(flexors_min[actual]> value){
        flexors_min[actual]= value;
       // Serial.println(String(flexors[actual])+"nuevo min: "+String(value));
      }
  }


// FUNCION PARA CALIBRAR LOS SENSORES
void calibrateFlexors()
{
  int value;
  boolean valid = false;
  if(calibrate==false){
        //Serial.println("Por favor, deje la mano estirada...");
        delay(2000);
        for(int i =0;i < count_sensors; i++){
          if(flexors[i]>-1){
            value=analogRead(flexors[i]);
            while(simpleValue(value)==-1){
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
              calcular_Max(i, value);
              calcular_Min(i, value);
             }
            }
        }
        delay(100);
    }
    calibrate=true;
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

//VERIFICA QUE EL DATO DEL SENSOR SEA VALIDO
int validValue(int value, int indice){
  if(value<90 || value>280){
    return -1;
  }
  else{
    value = readSensor(value,flexors_min[indice],flexors_max[indice]);
  //  Serial.println("resulta "+String(value));
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

void loop() {
  if(Serial.available() > 0){
      function = Serial.parseInt();
  }
  if(calibrate==false){
    for(int i =0;i < count_sensors; i++){
        if(flexors[i]>-1){
            flex_value=analogRead(flexors[i]);
            if(simpleValue(flex_value)>-1){
              Serial.println(String(flexors[i])+": "+String(flex_value));
            }
          }          
    }
  }else{
    for(int i =0;i < count_sensors; i++){
        if(flexors[i]>-1){
            flex_value=validValue(analogRead(flexors[i]), i);
            if(flex_value>-1){
             Serial.println(String(flexors[i])+": "+flex_value);
            }
          }       
    }
  //  Serial.println();   
  }
  if(function==9){
      calibrateFlexors();
      function=-1;
   }
   if(function==10){
      calibrate=false;
      calibrateFlexors();
      function=-1;
   }
  delay(60);
}
