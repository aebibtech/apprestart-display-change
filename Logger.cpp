#include "Logger.h"
#include <sstream>

std::mutex Logger::logMutex;

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("DisplayMonitor.log", std::ios_base::app);
    if (logFile.is_open()) {
        std::string logLine = GetTimestamp() + " [" + LevelToString(level) + "] " + message;
        logFile << logLine << std::endl;
        std::cout << logLine << std::endl;
    }
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
