#include <windows.h>
#include <shellapi.h>
#include "Logger.h"
#include "ProcessManager.h"
#include "ConfigManager.h"

Config g_config;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DISPLAYCHANGE) {
        Logger::Log(LogLevel::Info, "Display change detected (WM_DISPLAYCHANGE).");
        ProcessManager::RestartProcess(g_config.processName, g_config.processPath);
    } else if (uMsg == WM_SETTINGCHANGE) {
        if (lParam != 0 && lstrcmp((LPCTSTR)lParam, "DisplayConfig") == 0) {
            Logger::Log(LogLevel::Info, "Display setting change detected (WM_SETTINGCHANGE: DisplayConfig).");
            ProcessManager::RestartProcess(g_config.processName, g_config.processPath);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Convert WinMain command line to argc/argv format for ConfigManager
    int argc;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    // Convert wide-char args back to char* for the existing ConfigManager
    char** argv = new char*[argc];
    for (int i = 0; i < argc; i++) {
        int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
        argv[i] = new char[size];
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], size, NULL, NULL);
    }

    g_config = ConfigManager::LoadConfig(argc, argv);

    // Clean up temporary argv
    for (int i = 0; i < argc; i++) delete[] argv[i];
    delete[] argv;
    LocalFree(argvW);

    if (g_config.processName.empty() || g_config.processPath.empty()) {
        Logger::Log(LogLevel::Error, "Usage: DisplayMonitor.exe <process_name> <process_path> OR configure config.ini");
        return 1;
    }

    Logger::Log(LogLevel::Info, "Display Monitor started (Quiet Mode). Monitoring for changes...");

    // Create a hidden window to receive system messages
    const char CLASS_NAME[] = "DisplayMonitorWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Display Monitor Hidden Window",
        0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (hwnd == NULL) {
        Logger::Log(LogLevel::Error, "Failed to create message window.");
        return 1;
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
