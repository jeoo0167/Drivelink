#include "Logger.h"

Logger::Logger(const char* source) : source(source)
{

}

OutMode Logger::output_mode = OutMode::none;