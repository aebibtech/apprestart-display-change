#pragma once
#include <string>
#include <fstream>
#include <windows.h>

struct Config {
    std::string processName;
    std::string processPath;
};

class ConfigManager {
public:
    static Config LoadConfig(int argc, char* argv[]);
private:
    static Config LoadFromIni(const std::string& filename);
};
