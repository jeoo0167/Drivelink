#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
//#include "BluetoothSerial.h"

enum MsgType
{
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

enum OutMode
{
    serial,
    bluetooth,
    both,
    none
};

class Logger
{
public:

    Logger(const char* source);

    void init(String id = "Drivelink Device Debugger")
    {
        Serial.begin(baud_rate);
  //      SerialBT.begin(id);
    }

    static void setOutput(OutMode output)
    {
        output_mode = output;
    }

    template <typename Msg>
    void log_out(const Msg& msg)
    {
        switch(output_mode)
        {
            case OutMode::serial:
                Serial.print(msg);
                break;

            case OutMode::bluetooth:
                //SerialBT.print(msg);
                break;

            case OutMode::both:
                Serial.print(msg);
    //            SerialBT.print(msg);
                break;
            case OutMode::none:
                Serial.print("[LOGGER]: Select an output mode");
              //  SerialBT.print("[LOGGER]: Select an output mode");
                break;
        }
    }

    template <typename Msg>
    void log_out_ln(const Msg& msg)
    {
        switch(output_mode)
        {
            case OutMode::serial:
                Serial.println(msg);
                break;

            case OutMode::bluetooth:
      //          SerialBT.println(msg);
                break;

            case OutMode::both:
                Serial.println(msg);
        //        SerialBT.println(msg);
                break;
            case OutMode::none:
                Serial.print("[LOGGER]: Select an output mode\n");
          //      SerialBT.print("[LOGGER]: Select an output mode\n");
                break;
        }
    }

    template <typename Msg>
    void msg(MsgType msg_type, const Msg& msg)
    {
        log_out("[");
        log_out(source);
        log_out("][");

        switch(msg_type)
        {
            case MsgType::INFO:
                log_out("INFO");
                break;

            case MsgType::WARN:
                log_out("WARN");
                break;

            case MsgType::ERROR:
                log_out("ERROR");
                break;

            case MsgType::CRITICAL:
                log_out("CRITICAL");
                break;
        }

        log_out("]: ");
        log_out_ln(msg);
    }

private:

    int baud_rate = 115200;
    const char* source;
    static OutMode output_mode;

    //BluetoothSerial SerialBT;
};

#endif