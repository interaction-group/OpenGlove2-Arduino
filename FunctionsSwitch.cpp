#include "Arduino.h"
#include "FunctionsSwitch.h"
#include "GloveFunctions.h"
#include "GeneralFunctions.h"
#include "FlexorsFunctions.h"

int TIME_TEST_FUNCTION_NUMBER = 9;
unsigned long timeStart;

void selectFunction(int functionCase){
  switch (functionCase) {

        case 1:

          initializeMotor();
          break;

        case 2:
            
          activateMotor();
          break;

        case 3:

          analogRead();
          break;

        case 4:

          digitalRead();
          break;

        case 5:

          initializeDigitalInputs();
          break;

        case 6:

          pinMode();
          break;

        case 7:

          digitalWrite();
          break;

        case 8:

          analogWrite();
          break;

        case 9:

          timeStart = micros();
          activateMotorTimeTest(timeStart);
          break;
        
        default:

          break;
          
      }  
}

void selectFlexorsFunction(int functionCase, int flexors[], int count_sensors){
  switch (functionCase) {

        case 10:

          calibrateFlexors(flexors,count_sensors);
          break;

        case 11:
        
          set_threshold();
          break;

        default:

          break;
          
      }
  
}
