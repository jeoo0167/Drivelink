#ifndef DaTA_MANAGER_H
#define data_MANAGER_H

#include <LittleFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <array>
#include <Services/Logger.h>

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
    String Mac;
};

class FileManager
{
    public:
        FileManager();
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
        Logger logger;
};

class JsonManager 
{
    public:
        JsonManager();
        const Data& getData() const { return data; }
        Data& getData() { return data; }
        bool setMainData(FileID id);
        bool load(FileID id);
    private:
    Data data;
    JsonDocument doc;
    FileManager file_manager;
    Logger logger;
    bool loadMainSettings();
};

#endif