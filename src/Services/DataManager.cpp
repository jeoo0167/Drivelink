#include "DataManager.h"

FileManager::FileManager() : logger(__FILE__) {}

bool FileManager::begin()
{
    if(!LittleFS.begin())
    {
        logger.msg(MsgType::CRITICAL,"LITTLEFS SUCCESS INITED");
        return false;
    }
    logger.msg(MsgType::INFO,"LITTLEFS SUCCESS INITED");
    return true;
}

const char* FileManager::getPath(FileID file_id)
{
    for(const auto& file : path_file_list)
    {
        if(file.id == file_id)
        {
            return file.path;
        }
    }
    return nullptr;
}
template<typename Operation>
bool FileManager::operate(FileID id, const char*mode,Operation op)
{
    const char* path = getPath(id);
    if(path == nullptr) return false;
    File file = LittleFS.open(path,mode);
    if(!file) return false;
    bool result = op(file);
    file.close();
    return result;
}

bool FileManager::read(FileID file_id, String& data)
{
    return operate(file_id,"r",[&](File& file)
    {
        data = file.readString();
        return true;
    });
}

bool FileManager::write(FileID file_id, String& data)
{
    return operate(file_id,"a",[&](File& file)
    {
        return file.print(data) == data.length();
    });
}

bool FileManager::rewrite(FileID file_id, String& data)
{
    return operate(file_id,"w",[&](File& file)
    {
        return file.print(data) == data.length();
    });  
}


JsonManager::JsonManager() : logger(__FILE__) {}

bool JsonManager::load(FileID id)
{
    String json; 
    if(!file_manager.read(id,json)) return false;
    DeserializationError error = deserializeJson(doc,json);
    if(error)
    {
        logger.msg(MsgType::ERROR,error.c_str());
        return false;
    }

    if(!loadMainSettings())
    {
        logger.msg(MsgType::ERROR,"falied load main settings");
        return false;
    }
    return true;
}

bool JsonManager::loadMainSettings()
{
    if(doc["network"]["ssid"].isNull() || doc["network"]["password"].isNull() || 
    doc["Sounds"]["notification"].isNull() || doc["network"]["Mac"].isNull())
    {
        Serial.println("Json:data error");
        return false;
    }

    data.ssid = doc["network"]["ssid"].as<String>();
    data.password = doc["network"]["password"].as<String>();
    data.notification = doc["Sounds"]["notification"];
    data.Mac = doc["network"]["Mac"].as<String>();
    
    return true;
}

bool JsonManager::setMainData(FileID id)
{
    doc["network"]["ssid"] = data.ssid;
    doc["network"]["password"] = data.password;
    doc["network"]["Mac"] = data.Mac;
    doc["network"]["notification"] = data.notification;

    String json;
    serializeJson(doc,json);
    return file_manager.rewrite(id,json);
}