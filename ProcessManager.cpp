#include "ProcessManager.h"

bool ProcessManager::RestartProcess(const std::string& processName, const std::string& processPath) {
    Logger::Log(LogLevel::Info, "Attempting to restart process: " + processName);
    
    DWORD pid = FindProcessId(processName);
    if (pid != 0) {
        Logger::Log(LogLevel::Info, "Found existing process with PID: " + std::to_string(pid));
        if (TerminateProcessById(pid)) {
            Logger::Log(LogLevel::Info, "Successfully terminated process: " + processName);
        } else {
            Logger::Log(LogLevel::Error, "Failed to terminate process: " + processName);
            return false;
        }
    } else {
        Logger::Log(LogLevel::Warning, "Process " + processName + " not found running.");
    }

    const int maxRetries = 3;
    const int verifyAttempts = 5;
    const DWORD verifyDelayMs = 200;
    const DWORD retryDelayMs = 1000;

    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        if (attempt > 1) {
            DWORD existingPid = FindProcessId(processName);
            if (existingPid != 0) {
                Logger::Log(LogLevel::Info, "Process is already running (detected late) with PID: " + std::to_string(existingPid));
                return true;
            }
        }

        Logger::Log(LogLevel::Info, "Starting process: " + processPath + " (Attempt " + std::to_string(attempt) + " of " + std::to_string(maxRetries) + ")");
        if (StartNewProcess(processPath)) {
            DWORD newPid = 0;
            for (int verify = 0; verify < verifyAttempts; ++verify) {
                newPid = FindProcessId(processName);
                if (newPid != 0) {
                    break;
                }
                Sleep(verifyDelayMs);
            }

            if (newPid != 0) {
                Logger::Log(LogLevel::Info, "Successfully started process: " + processPath + " (PID: " + std::to_string(newPid) + ")");
                return true;
            } else {
                Logger::Log(LogLevel::Warning, "Process started but PID for " + processName + " was not found on attempt " + std::to_string(attempt));
            }
        } else {
            Logger::Log(LogLevel::Error, "Failed to start process: " + processPath + " on attempt " + std::to_string(attempt));
        }

        if (attempt < maxRetries) {
            Logger::Log(LogLevel::Info, "Retrying in " + std::to_string(retryDelayMs) + "ms...");
            Sleep(retryDelayMs);
        }
    }

    Logger::Log(LogLevel::Error, "Failed to start process after " + std::to_string(maxRetries) + " attempts.");
    return false;
}

DWORD ProcessManager::FindProcessId(const std::string& processName) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (processName == pe32.szExeFile) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

bool ProcessManager::TerminateProcessById(DWORD dwProcessId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, dwProcessId);
    if (hProcess == NULL) {
        // Fallback to only PROCESS_TERMINATE if SYNCHRONIZE is not permitted
        hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
        if (hProcess == NULL) return false;
        BOOL result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result != 0;
    }

    BOOL result = TerminateProcess(hProcess, 0);
    if (result) {
        // Wait up to 5000 milliseconds for the process to exit
        WaitForSingleObject(hProcess, 5000);
    }
    CloseHandle(hProcess);
    return result != 0;
}

bool ProcessManager::StartNewProcess(const std::string& processPath) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Wrap the path in quotes to handle spaces correctly
    std::string quotedPath = "\"" + processPath + "\"";

    char* cmdLine = new char[quotedPath.length() + 1];
    strcpy_s(cmdLine, quotedPath.length() + 1, quotedPath.c_str());

    bool success = false;
    if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        success = true;
    } else {
        DWORD error = GetLastError();
        Logger::Log(LogLevel::Error, "CreateProcess failed with error code: " + std::to_string(error));
    }

    delete[] cmdLine;
    return success;
}
