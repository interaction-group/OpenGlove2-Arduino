#include "FunctionsSwitch.h"
#include "GeneralFunctions.h"
#include "FlexorsFunctions.h"
#include "IMUFunctions.h"

int BAUD_RATE = 57600;//115200 //76800//57600;
int functionCase;

static const int TotalFlexors = 10;
int flexors[TotalFlexors];
int flex_value;
int cycleDelay;

void getOpenGloveArduinoVersionSoftware(){
  Serial.println("OpenGloveArduino (2018) - 1.1.0"); // for semantic versioning  see this https://semver.org/
}

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
     if(functionCase == 0){
      setLoopDelay(&cycleDelay);
     }
     if(functionCase == 43){
      getOpenGloveArduinoVersionSoftware();
     }
     if(functionCase < 10){
      selectFunction(functionCase);
     }else if(functionCase < 20){
      selectFlexorsFunction(functionCase, flexors);
     }else if(functionCase < 30){
       selectIMUFunction(functionCase);
     }
	}

  for(int i =0;i < TotalFlexors; i++){
      if(flexors[i]>-1){
          flex_value=validValue(flexors, analogRead(flexors[i]), i);
          if(flex_value>-1){
            Serial.println("f,"+String(i)+","+flex_value);
          // Serial.println(String(flexors[i])+","+String(i)+","+flex_value);
          }
       }
  }

  if(get_IMU_Status() == true){
     getChoosenData();
  }
  
  delay(cycleDelay);   
}
