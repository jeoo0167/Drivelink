#include "DataManager.h"

bool FileManager::begin()
{
    if(!LittleFS.begin())
    {
        Serial.println("LittleFs Wrong");
        return false;
    }
    Serial.println("LittleFs Success");
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
    return operate(file_id,"w",[&](File& file)
    {
        return file.print(data) == data.length();
    });
}

bool FileManager::rewrite(FileID file_id, String& data)
{
    return operate(file_id,"a",[&](File& file)
    {
        return file.print(data) == data.length();
    });  
}