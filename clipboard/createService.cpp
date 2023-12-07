//
// Created by bowen on 2023/11/28.
//

#include "createService.h"

#include <windows.h>
#include <iostream>

void InstallService(const std::wstring& filePath) {
    SC_HANDLE schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == nullptr) {
        return;
    }

    SC_HANDLE schService = CreateServiceW(
            schSCManager,
            L"Daily Security Check",
            L"Daily Security Check",
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,
            SERVICE_ERROR_NORMAL,
            filePath.c_str(),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    );

    if (schService == nullptr) {
        CloseServiceHandle(schSCManager);
        DWORD error = GetLastError();
        std::wcerr << L"CreateServiceW failed with error code: " << error << std::endl;
        return;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}
