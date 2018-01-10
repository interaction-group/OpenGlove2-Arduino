#include "Arduino.h"
#include "IMUFunctions.h"
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

// Earth's magnetic field varies by location. Add or subtract a declination to get a more accurate heading. Calculate your's here: http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -1.44 // Declination (degrees) in Boulder, CO =  -8.58, Santiago, Chile = 1.44

LSM9DS1 imu;
boolean IMU_Status = false;
bool raw_values = false;
static char TERMINAL_SIGN = 's';

void setup_IMU()
{
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
}

void start_IMU(){
  if(Serial.read()==TERMINAL_SIGN){
    setup_IMU();
    if(imu.begin()){
      IMU_Status=true;
    }else{
      IMU_Status=false;
    }
  }
}

void set_RawData()
{
  int value = Serial.parseInt();
  if(Serial.read()==TERMINAL_SIGN)
  {
    if(value==1)
    {
      raw_values=true;
    }else
    {
      raw_values=false;
    }
  }
}

boolean get_IMU_Status()
{
  return IMU_Status;
}

void set_IMU_Status()
{
  int option = Serial.parseInt();
  if(Serial.read()==TERMINAL_SIGN){
    if(option==1){
      IMU_Status=true;
      }
    else{
      IMU_Status=false;
      }
  }
}

void getAllData(){
  if(imu.accelAvailable() && imu.gyroAvailable() && imu.magAvailable())
  {
    imu.readAccel();
    imu.readGyro();
    imu.readMag();
    if(raw_values==true)
    {
      Serial.println("z,"+String(imu.ax)+","+String(imu.ay)+","+String(imu.az)+","+
                          String(imu.gx)+","+String(imu.gy)+","+String(imu.gz)+","+
                          String(imu.mx)+","+String(imu.my)+","+String(imu.mz));
    }else
    {
      Serial.println("z,"+String(imu.calcAccel(imu.ax))+","+String(imu.calcAccel(imu.ay))+","+String(imu.calcAccel(imu.az))+","+
                          String(imu.calcAccel(imu.gx))+","+String(imu.calcAccel(imu.gy))+","+String(imu.calcAccel(imu.gz))+","+
                          String(imu.calcAccel(imu.mx))+","+String(imu.calcAccel(imu.my))+","+String(imu.calcAccel(imu.mz)));
    }
  }
/*  if(imu.gyroAvailable())
  {
    imu.readGyro();
  }
  if(imu.magAvailable())
  {
    imu.readMag();
  }

  if(raw_values==true)
  {
    Serial.println("z,"+String(imu.ax)+","+String(imu.ay)+","+String(imu.az)+","+
                        String(imu.gx)+","+String(imu.gy)+","+String(imu.gz)+","+
                        String(imu.mx)+","+String(imu.my)+","+String(imu.mz));
  }else
  {
    Serial.println("z,"+String(imu.calcAccel(imu.ax))+","+String(imu.calcAccel(imu.ay))+","+String(imu.calcAccel(imu.az))+","+
                        String(imu.calcAccel(imu.gx))+","+String(imu.calcAccel(imu.gy))+","+String(imu.calcAccel(imu.gz))+","+
                        String(imu.calcAccel(imu.mx))+","+String(imu.calcAccel(imu.my))+","+String(imu.calcAccel(imu.mz)));
  }
  */
}

void getGyro_raw(){
  imu.readGyro();
  Serial.println("g,"+String(imu.gx)+","+String(imu.gy)+","+String(imu.gz));
}

void getAccel_raw(){
  imu.readAccel();
  Serial.println("g,"+String(imu.ax)+","+String(imu.ay)+","+String(imu.az));
}

void getMag_raw(){
  imu.readMag();
  Serial.println("g,"+String(imu.mx)+","+String(imu.my)+","+String(imu.mz));
}

void getGyro_dps(){
 // imu.readGyro();
  //Serial.println("g,"+imu.gx+","+imu.gy+","+imu.gz);
}

void getAttitude(){
  calculeAttitude(imu.ax, imu.ay, imu.az, 
                 -imu.my, -imu.mx, imu.mz);
}

void calculeAttitude(float ax, float ay, float az, float mx, float my, float mz){
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;
  Serial.print("r,"+String(pitch)+","+String(roll)+","+String(heading));
}

