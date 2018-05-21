#include "Arduino.h"
#include "IMUFunctions.h"
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

// Earth's magnetic field varies by location. Add or subtract a declination to get a more accurate heading. Calculate your's here: http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION 1.44 // Declination (degrees) in Boulder, CO =  -8.58, Santiago, Chile = 1.44

#include <MadgwickAHRS.h>
Madgwick filter;
unsigned long microsPerReading, microsPrevious;

LSM9DS1 imu;
boolean IMU_Status = false;
bool raw_values = false;
static char TERMINAL_SIGN = 's';
int imuMode = 'z';

void setupGyro()
{
  // [enabled] turns the gyro on or off.
  imu.settings.gyro.enabled = true;  // Enable the gyro
  // [scale] sets the full-scale range of the gyroscope.
  // scale can be set to either 245, 500, or 2000
  imu.settings.gyro.scale = 245; // Set scale to +/-245dps
  // [sampleRate] sets the output data rate (ODR) of the gyro
  // sampleRate can be set between 1-6
  // 1 = 14.9    4 = 238
  // 2 = 59.5    5 = 476
  // 3 = 119     6 = 952
  imu.settings.gyro.sampleRate = 3; // 59.5Hz ODR
  // [bandwidth] can set the cutoff frequency of the gyro.
  // Allowed values: 0-3. Actual value of cutoff frequency
  // depends on the sample rate. (Datasheet section 7.12)
  imu.settings.gyro.bandwidth = 0;
  // [lowPowerEnable] turns low-power mode on or off.
  imu.settings.gyro.lowPowerEnable = false; // LP mode off
  // [HPFEnable] enables or disables the high-pass filter
  imu.settings.gyro.HPFEnable = true; // HPF disabled
  // [HPFCutoff] sets the HPF cutoff frequency (if enabled)
  // Allowable values are 0-9. Value depends on ODR.
  // (Datasheet section 7.14)
  imu.settings.gyro.HPFCutoff = 1; // HPF cutoff = 4Hz
  // [flipX], [flipY], and [flipZ] are booleans that can
  // automatically switch the positive/negative orientation
  // of the three gyro axes.
  imu.settings.gyro.flipX = false; // Don't flip X
  imu.settings.gyro.flipY = false; // Don't flip Y
  imu.settings.gyro.flipZ = false; // Don't flip Z
}

void setupAccel()
{
  // [enabled] turns the acclerometer on or off.
  imu.settings.accel.enabled = true; // Enable accelerometer
  // [enableX], [enableY], and [enableZ] can turn on or off
  // select axes of the acclerometer.
  imu.settings.accel.enableX = true; // Enable X
  imu.settings.accel.enableY = true; // Enable Y
  imu.settings.accel.enableZ = true; // Enable Z
  // [scale] sets the full-scale range of the accelerometer.
  // accel scale can be 2, 4, 8, or 16
  imu.settings.accel.scale = 2; // Set accel scale to +/-8g.
  // [sampleRate] sets the output data rate (ODR) of the
  // accelerometer. ONLY APPLICABLE WHEN THE GYROSCOPE IS
  // DISABLED! Otherwise accel sample rate = gyro sample rate.
  // accel sample rate can be 1-6
  // 1 = 10 Hz    4 = 238 Hz
  // 2 = 50 Hz    5 = 476 Hz
  // 3 = 119 Hz   6 = 952 Hz
  imu.settings.accel.sampleRate = 1; // Set accel to 10Hz.
  // [bandwidth] sets the anti-aliasing filter bandwidth.
  // Accel cutoff freqeuncy can be any value between -1 - 3. 
  // -1 = bandwidth determined by sample rate
  // 0 = 408 Hz   2 = 105 Hz
  // 1 = 211 Hz   3 = 50 Hz
  imu.settings.accel.bandwidth = 0; // BW = 408Hz
  // [highResEnable] enables or disables high resolution 
  // mode for the acclerometer.
  imu.settings.accel.highResEnable = false; // Disable HR
  // [highResBandwidth] sets the LP cutoff frequency of
  // the accelerometer if it's in high-res mode.
  // can be any value between 0-3
  // LP cutoff is set to a factor of sample rate
  // 0 = ODR/50    2 = ODR/9
  // 1 = ODR/100   3 = ODR/400
  imu.settings.accel.highResBandwidth = 0;  
}

void setupMag()
{
  // [enabled] turns the magnetometer on or off.
  imu.settings.mag.enabled = true; // Enable magnetometer
  // [scale] sets the full-scale range of the magnetometer
  // mag scale can be 4, 8, 12, or 16
  imu.settings.mag.scale = 12; // Set mag scale to +/-12 Gs
  // [sampleRate] sets the output data rate (ODR) of the
  // magnetometer.
  // mag data rate can be 0-7:
  // 0 = 0.625 Hz  4 = 10 Hz
  // 1 = 1.25 Hz   5 = 20 Hz
  // 2 = 2.5 Hz    6 = 40 Hz
  // 3 = 5 Hz      7 = 80 Hz
  imu.settings.mag.sampleRate = 5; // Set OD rate to 20Hz
  // [tempCompensationEnable] enables or disables 
  // temperature compensation of the magnetometer.
  imu.settings.mag.tempCompensationEnable = false;
  // [XYPerformance] sets the x and y-axis performance of the
  // magnetometer to either:
  // 0 = Low power mode      2 = high performance
  // 1 = medium performance  3 = ultra-high performance
  imu.settings.mag.XYPerformance = 3; // Ultra-high perform.
  // [ZPerformance] does the same thing, but only for the z
  imu.settings.mag.ZPerformance = 3; // Ultra-high perform.
  // [lowPowerEnable] enables or disables low power mode in
  // the magnetometer.
  imu.settings.mag.lowPowerEnable = false;
  // [operatingMode] sets the operating mode of the
  // magnetometer. operatingMode can be 0-2:
  // 0 = continuous conversion
  // 1 = single-conversion
  // 2 = power down
  imu.settings.mag.operatingMode = 0; // Continuous mode
}


void setupDevice()
{
  // [commInterface] determines whether we'll use I2C or SPI
  // to communicate with the LSM9DS1.
  // Use either IMU_MODE_I2C or IMU_MODE_SPI
  imu.settings.device.commInterface = IMU_MODE_I2C;
  // [mAddress] sets the I2C address or SPI CS pin of the
  // LSM9DS1's magnetometer.
  imu.settings.device.mAddress = 0x1E; // Use I2C addres 0x1E
  // [agAddress] sets the I2C address or SPI CS pin of the
  // LSM9DS1's accelerometer/gyroscope.
  imu.settings.device.agAddress = 0x6B; // I2C address 0x6B
}

void setup_IMU()
{
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
 imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  //setupDevice();
 // setupGyro(); // Set up gyroscope parameters
  //setupAccel(); // Set up accelerometer parameters
  //setupMag(); // Set up magnetometer parameters
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
  filter.begin(25);
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

void set_ChoosingData()
{
  int value = Serial.parseInt();
  if(Serial.read()==TERMINAL_SIGN)
  {
    switch (value) {
        case 0:
          imuMode = 'a';
          break;
        case 1: 
          imuMode = 'g';
          break;
        case 2:
          imuMode = 'm';
          break;
        case 3:
          imuMode = 'r';
          break;
        default:
          imuMode = 'z';
          break;
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

void getChoosenData(){
    if(imuMode == 'z'){
      getAllData();
    }
    if(imuMode == 'g'){
      getGyroData();
    }
    if(imuMode == 'a'){
      getAccelData();
    }
    if(imuMode == 'm'){
      getMagData();
    }
    if(imuMode == 'r'){
      //getAttitude();
      getAttitude2();
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
                          String(imu.calcGyro(imu.gx))+","+String(imu.calcGyro(imu.gy))+","+String(imu.calcGyro(imu.gz))+","+
                          String(imu.calcMag(imu.mx))+","+String(imu.calcMag(imu.my))+","+String(imu.calcMag(imu.mz)));
    }
  }
}

void getAttitude(){
  if(imu.accelAvailable() && imu.magAvailable())
  {
    imu.readAccel();
    imu.readMag();
    calculeAttitude(imu.ax, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz);
  }
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
  //heading *= 180.0 / PI;
  //pitch *= 180.0 / PI;
  //roll  *= 180.0 / PI;
  Serial.println("r,"+String(pitch)+","+String(roll)+","+String(heading));
}


void getGyroData(){
  if(imu.gyroAvailable())
  {
    imu.readGyro();
    if(raw_values==true)
    {
      Serial.println("g,"+String(imu.gx)+","+String(imu.gy)+","+String(imu.gz));
    }else
    {
      Serial.println("g,"+String(imu.calcGyro(imu.gx))+","+String(imu.calcGyro(imu.gy))+","+String(imu.calcGyro(imu.gz)));
    }
    
  }
}

void getAccelData(){
  if(imu.accelAvailable())
  {
    imu.readAccel();
    if(raw_values==true)
    {
      Serial.println("a,"+String(imu.ax)+","+String(imu.ay)+","+String(imu.az));
    }else
    {
      Serial.println("a,"+String(imu.calcAccel(imu.ax))+","+String(imu.calcAccel(imu.ay))+","+String(imu.calcAccel(imu.az)));
    }
    
  }
}

void getMagData(){
  if(imu.magAvailable())
  {
    imu.readMag();
    if(raw_values==true)
    {
      Serial.println("m,"+String(imu.mx)+","+String(imu.my)+","+String(imu.mz));
    }else
    {
      Serial.println("m,"+String(imu.calcMag(imu.mx))+","+String(imu.calcMag(imu.my))+","+String(imu.calcMag(imu.mz)));
    }
    
  }
}

void getAttitude2(){
  float roll, pitch, yaw;
  unsigned long microsNow;

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    imu.readGyro();
    imu.readAccel();
    imu.readMag();

    float gx, gy, gz, ax, ay, az, mx, my, mz;

    // Use a right-handed coordinate system
    // secondary -/+ from calibration
    
    gx = imu.calcGyro(imu.gy) - 0.3;
    gy = imu.calcGyro(imu.gx) - 1.0;
    gz = imu.calcGyro(imu.gz) + 3.0;

    ax = imu.calcAccel(imu.ay);
    ay = imu.calcAccel(imu.ax);
    az = imu.calcAccel(imu.az) - 1.0;

    mx = imu.calcMag(imu.my);
    my = -imu.calcMag(imu.mx);
    mz = imu.calcMag(imu.mz);

    // update the filter, which computes orientation
    filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

    // print the heading, pitch and roll
    roll = filter.getRollRadians();
    pitch = filter.getPitchRadians();
    yaw = filter.getYawRadians();
    Serial.println("r,"+String(pitch)+","+String(roll)+","+String(yaw));

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}



