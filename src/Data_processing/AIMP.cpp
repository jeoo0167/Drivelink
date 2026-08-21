#include "hardware/IMU.h"
#include "AIMP.h"
#include <Arduino.h>

PositionModel::PositionModel(IMU &imu) : imu(imu) {}

void PositionModel::showClases()
{
    switch(position_info.predicted_class)
    {
        case 0:
            Serial.print("STOP: ");
            Serial.println(position_info.confidence);
            break;
        case 1:
            Serial.print("Forward ");
            Serial.println(position_info.confidence);
            break;
        case 2:
            Serial.print("Backward ");
            Serial.println(position_info.confidence);
            break;
        case 3:
            Serial.print("Left ");
            Serial.println(position_info.confidence);
            break;
        case 4:
            Serial.print("Right ");
            Serial.println(position_info.confidence);
            break;
        default:
            Serial.println("Unknown class");
    }
}


void PositionModel::predictPosition()
{
    a0[0] = normalize(imu.getData().raw.ax,mean[0],dstd[0]);
    a0[1] = normalize(imu.getData().raw.ay,mean[1],dstd[1]);
    a0[2] = normalize(imu.getData().raw.az,mean[2],dstd[2]);


    for(int i = 0 ; i<10; i++ ) 
    {
        aux=0.0;
        for(int j = 0 ; j <3 ; j++ ) 
        {
             aux=aux+W1[i][j]*a0[j];
        } 
        a1[i]=relu(aux+b1[i]);
    }
    for(int i = 0 ; i<10; i++ ) 
    {
        aux=0.0;
        for(int j = 0 ; j <10 ; j++ ) 
        {
             aux=aux+W2[i][j]*a1[j];
        } 
        a2[i]=relu(aux+b2[i]);}
    double aux1 = 0;
    for(int i = 0 ; i<5; i++ ) 
    {
        aux=0.0;
        for(int j = 0 ; j <10 ; j++ )
        { 
            aux=aux+W3[i][j]*a2[j];
        }
        a3[i]=(aux+b3[i]);
        aux1=aux1+exp(a3[i]);
    }
    double minimo = 0.0;
    int classes = 0;
    for(int i = 0;  i<5; i++)
    {
        a3[i] = exp(a3[i])/aux1;
        if(a3[i]>minimo)
        {
            minimo=a3[i];classes=i;
        }
    }
    position_info.predicted_class = classes;
    position_info.confidence = minimo;
}

double PositionModel::normalize(double input, double mean, double stddev)
{
    return (input-mean)/stddev;
}

double PositionModel::relu(double x)
{
    return (x > 0) ? x : 0.0;
}