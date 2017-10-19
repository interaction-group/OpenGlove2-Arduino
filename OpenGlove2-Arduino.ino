static char TERMINAL_SIGN = 's';
int flexors[10];
int flexors_min[10];
int flexors_max[10];
int function;
boolean calibrate;
int count_sensors;
int flex_value;
void setup() {
  Serial.begin(57600);
  for(int i =0;i < 10; i++){
      flexors[i]=-1;
  }
  flexors[0]=16;
  flexors[1]=17;
  flexors[2]=19;
  count_sensors=3;

  for(int i =0;i < count_sensors; i++){
    if(flexors[i]>-1){
      pinMode(flexors[i],INPUT);
    }
  }
  calibrate=false;
}

void calcular_Max(int actual, int value){
    if(flexors_max[actual]< value){
        flexors_max[actual]= value;
       // Serial.println(String(flexors[actual])+"nuevo max: "+String(value));
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
            while(validValue(value)==-1){
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
             if(validValue(value)>-1){
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
     return map(sensorValue, r_max, r_min,0, 100);
}

//VERIFICA QUE EL DATO DEL SENSOR SEA VALIDO
int validValue(int value){
  if(value<90 || value>280){
    return -1;
  }
  else{
    return value;
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
            if(validValue(flex_value)>-1){
              Serial.println(String(flexors[i])+": "+String(flex_value));
            }
          }          
    }
  }else{
    for(int i =0;i < count_sensors; i++){
        if(flexors[i]>-1){
            flex_value=analogRead(flexors[i]);
            if(validValue(flex_value)>-1){
             Serial.println(String(flexors[i])+": "+String(readSensor(flex_value,flexors_min[i],flexors_max[i])));
            }
          }        
    }
  //  Serial.println();   
  }

   if(function==10){
      calibrate=false;
      calibrateFlexors();
      function=-1;
   }
  delay(100);
}
