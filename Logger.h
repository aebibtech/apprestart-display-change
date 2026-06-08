#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void Log(LogLevel level, const std::string& message);
private:
    static std::string LevelToString(LogLevel level);
    static std::string GetTimestamp();
    static std::mutex logMutex;
};
