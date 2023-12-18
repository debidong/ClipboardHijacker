//
// Created by bowen on 2023/11/28.
//

#ifndef CLIPBOARD_CREATESERVICE_H
#define CLIPBOARD_CREATESERVICE_H
#include "windows.h"
#include <string>
void __stdcall runService(DWORD argc, LPWSTR *argv);
void InstallService(const std::wstring& filePath);
void WINAPI ServiceCtrlHandler(DWORD CtrlCode);

#endif //CLIPBOARD_CREATESERVICE_H