#pragma once
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include "Logger.h"

class ProcessManager {
public:
    static bool RestartProcess(const std::string& processName, const std::string& processPath);
private:
    static DWORD FindProcessId(const std::string& processName);
    static bool TerminateProcessById(DWORD dwProcessId);
    static bool StartNewProcess(const std::string& processPath);
};
