#ifndef IMU_H
#define IMU_H

#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>

struct Data_Raw
{
    int16_t ax,ay,az; 
    int16_t gx,gy,gz;
};

struct Data_Cuaternions
{
    float a,i,j,k;
};

enum class AngleUnit
{
    RADIANS,
    DEGREES
};
struct Data_EulerAngles
{
    double yaw,pitch,roll;
};

struct IMU_Data
{
    Data_Raw raw;
    Data_Cuaternions cuaternion;
    Data_EulerAngles euler_angles;
};
const double IMU_DEG = 180.0 / M_PI;
class IMU
{
    public:
        const IMU_Data& getData() const { return imu_data; }
        void begin();
        void update();
        void setAngleUnit(AngleUnit unit);

    private:
        MPU6050 mpu;

        int sda_pin = 21;
        int scl_pin = 22;

        void Calibration();
        float AccelOffsetX;
        float AccelOffsetY;
        float AccelOffsetZ;
        float GyroOffsetX;
        float GyroOffsetY;
        float GyroOffsetZ;

        bool calibred=false;

        bool dmpReady = false;
        uint8_t devStatus;
        uint16_t packetSize;
        uint8_t fifoBuffer[64];

        Quaternion q;
        VectorFloat gravity;
        float ypr[3];
        AngleUnit angleUnit = AngleUnit::DEGREES;

        IMU_Data imu_data;
        
};

#endif