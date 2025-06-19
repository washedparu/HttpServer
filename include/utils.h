#pragma once
#include <string>
#include <sstream>
#include <cstring>

// that should be the name of the project i think.
namespace HttpServer
{
    bool endsWith(const std::string& str, const std::string& suffix); 
    std::string getMimeType(const std::string& path);
    std::string getClientIPFromHeaders(const std::string& request); 

}