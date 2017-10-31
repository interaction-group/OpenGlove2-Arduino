#include "FunctionsSwitch.h"
#include "FlexorsFunctions.h"

int BAUD_RATE = 57600;
int functionCase;

static const int TotalFlexors = 10;
int flexors[TotalFlexors];
int flex_value;
int cycleDelay;
void setup() {
 
 	Serial.begin(57600);

  for(int i =0;i < TotalFlexors; i++){
    flexors[i]=-1;
  }
  cycleDelay=60;
}

void loop() {

	if (Serial.available() > 0) {
	    functionCase = Serial.parseInt();
     if(functionCase<10){
      selectFunction(functionCase);
     }else if(functionCase<20){
      selectFlexorsFunction(functionCase, flexors);
     }
	}

  for(int i =0;i < TotalFlexors; i++){
      if(flexors[i]>-1){
          flex_value=validValue(flexors, analogRead(flexors[i]), i);
          if(flex_value>-1){
            Serial.println(String(i)+","+flex_value);
          // Serial.println(String(flexors[i])+","+String(i)+","+flex_value);
          }
       }    
  }
  
  delay(cycleDelay);   
}
