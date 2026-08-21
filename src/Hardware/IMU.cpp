#include "IMU.h"

void IMU::begin()
{
    Wire.begin();
    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    if(mpu.testConnection())
    {
        Serial.println("MPU SUCCESS");
    }
    else 
    {
        Serial.print("MPU WRONG");
        while (true);
    }


    if (devStatus == 0)
    {
        mpu.setDMPEnabled(true);

        dmpReady = true;

        packetSize = mpu.dmpGetFIFOPacketSize();

        Serial.println("DMP Success");
    }
    else
    {
        Serial.print("DMP Wrong: ");
        Serial.println(devStatus);
    }
}

void IMU::update()//  !
{    
    mpu.getMotion6(&imu_data.raw.ax, &imu_data.raw.ay, &imu_data.raw.az,
                   &imu_data.raw.gx,&imu_data.raw.gy,&imu_data.raw.gz);

    if (!dmpReady)
        return;
    if (mpu.getFIFOCount() < packetSize)
        return;
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    mpu.dmpGetQuaternion(&q,fifoBuffer);
    imu_data.cuaternion.a = q.w;
    imu_data.cuaternion.i = q.x;
    imu_data.cuaternion.j = q.y;
    imu_data.cuaternion.k = q.z;

    mpu.dmpGetYawPitchRoll(ypr,&q,&gravity);
    
    imu_data.euler_angles.yaw = (angleUnit == AngleUnit::DEGREES) ? ypr[0] * IMU_DEG : ypr[0];
    imu_data.euler_angles.pitch = (angleUnit == AngleUnit::DEGREES) ? ypr[1] * IMU_DEG : ypr[1];
    imu_data.euler_angles.roll = (angleUnit == AngleUnit::DEGREES) ? ypr[2] * IMU_DEG : ypr[2];
}

void IMU::setAngleUnit(AngleUnit unit)
{
    angleUnit = unit;
}

