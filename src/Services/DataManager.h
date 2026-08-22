#ifndef DaTA_MANAGER_H
#define data_MANAGER_H

#include <LittleFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <array>
enum FileID
{
    Settings,
    Calibration
};
struct File_info
{
    FileID id;
    const char* path;
};

struct Data
{
    String ssid;
    String password;
    bool notification;
    const char* Mac;
};

class JsonManager 
{
    public:
        void begin();
        const Data& getData() const { return data; }
    private:
    Data data;
    JsonDocument doc;
};

class FileManager
{
    public:
        bool begin();
        const char* getPath(FileID file_id);
        bool write(FileID file_id, String& data);
        bool rewrite(FileID file_id, String& data);
        bool read(FileID file_id, String& data);
    private:
        std::array<File_info, 2> path_file_list ={ 
        {
          {FileID::Settings, "/Json/Settings.json"},
          {FileID::Calibration, "/Json/Calib_data.json"} 
        }};
        File_info f_inf;
        template<typename Operation>
        bool operate(FileID id, const char*mode,Operation op);
};

#endif