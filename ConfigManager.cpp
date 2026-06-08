#include "ConfigManager.h"
#include "Logger.h"

Config ConfigManager::LoadConfig(int argc, char* argv[]) {
    Config config;
    if (argc >= 3) {
        config.processName = argv[1];
        config.processPath = argv[2];
        Logger::Log(LogLevel::Info, "Loaded config from CLI arguments.");
    } else {
        config = LoadFromIni("config.ini");
        if (config.processName.empty() || config.processPath.empty()) {
            Logger::Log(LogLevel::Warning, "Config incomplete or config.ini missing.");
        } else {
            Logger::Log(LogLevel::Info, "Loaded config from config.ini.");
        }
    }
    return config;
}

Config ConfigManager::LoadFromIni(const std::string& filename) {
    Config config;
    char nameBuf[256];
    char pathBuf[MAX_PATH];

    GetPrivateProfileString("Config", "ProcessName", "", nameBuf, sizeof(nameBuf), ("./" + filename).c_str());
    GetPrivateProfileString("Config", "ProcessPath", "", pathBuf, sizeof(pathBuf), ("./" + filename).c_str());

    config.processName = nameBuf;
    config.processPath = pathBuf;

    return config;
}
