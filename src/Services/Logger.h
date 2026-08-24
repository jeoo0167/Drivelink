#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum MsgType
{
    INFO,
    WARN,
    ERROR,
    CRITICAL
};


class Logger
{
    public:
        Logger(const char* source);
        template <typename Msg>
        void msg(MsgType msg_type,const Msg& msg)
        {
            Serial.print("[");
            Serial.print(source);
            Serial.print("][");
            switch(msg_type)
            {
                case MsgType::INFO:
                    Serial.print("INFO");
                    break;
                case MsgType::WARN:
                    Serial.print("WARN");
                    break;
                case MsgType::ERROR:
                    Serial.print("ERROR");
                    break;
                case MsgType::CRITICAL:
                    Serial.print("CRITICAL");
                    break;
            }
            Serial.print("]: ");
            Serial.println(msg);
        }
    private:
        const char* source;
    
};

#endif