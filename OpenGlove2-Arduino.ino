#include "FunctionsSwitch.h"
#include "FlexorsFunctions.h"

int BAUD_RATE = 57600;
int functionCase;

static const int TotalFlexors = 10;
int flexors[TotalFlexors];
int count_sensors, flex_value;

void setup() {
 
 	Serial.begin(57600);

  for(int i =0;i < TotalFlexors; i++){
    flexors[i]=-1;
  }

  flexors[0]=16;
  flexors[1]=17;
  flexors[2]=19;
  count_sensors=3; 	
}

void loop() {

	if (Serial.available() > 0) {
	    functionCase = Serial.parseInt();
     if(functionCase<10){
      selectFunction(functionCase);
     }else if(functionCase<20){
      selectFlexorsFunction(functionCase, flexors, count_sensors);
        
      }
	}
 
   if(get_calibrationStatus()==false){
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
            flex_value=validValue(flexors, analogRead(flexors[i]), i);
            if(flex_value>-1){
             Serial.println(String(flexors[i])+": "+flex_value);
            }
         }     
    }
  }
  delay(60);   
}
