//
// Created by bowen on 2023/12/6.
//

#include "anti.h"
#define KEY_WOW64_64KEY 0x0100


BOOL queryRegistryEntries(HKEY hKey, const char* virtualMachineStrings[]);

/*
 * Uses NtQueryKey and RegOpenKeyExW to query registry information, including \HKLM\System\CurrentControlSet\Enum\IDE
 * and \HKLM\System\CurrentControlSet\Enum\SCSI, comparing with a list of strings associated with virtual machines.
 * Searches and detects specified registry entries related to virtual environments.
 * */


// Function to check if the system is running on a virtual machine
BOOL checkReg() {
    HKEY hKey;
    DWORD buffer_size = 1024;
    WCHAR buffer[1024];
    LONG result;

    // List of strings associated with virtual machines
    const char* virtualMachineStrings[] = { "VMware", "VirtualBox", "QEMU", "Hyper-V" };

    // Check IDE registry entries
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Enum\\IDE", 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
    if (result == ERROR_SUCCESS) {
        if (queryRegistryEntries(hKey, virtualMachineStrings)) {
            RegCloseKey(hKey);
            return 1;  // Virtual machine detected
        }
        RegCloseKey(hKey);
    }

    // Check SCSI registry entries
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Enum\\SCSI", 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
    if (result == ERROR_SUCCESS) {
        if (queryRegistryEntries(hKey, virtualMachineStrings)) {
            RegCloseKey(hKey);
            return 1;  // Virtual machine detected
        }
        RegCloseKey(hKey);
    }

    return 0;  // No virtual machine detected
}

BOOL queryRegistryEntries(HKEY hKey, const char* virtualMachineStrings[]) {
    DWORD index = 0;
    DWORD valueNameSize, valueType, valueDataSize;
    WCHAR valueName[1024];
    BYTE valueData[1024];

    while (true) {
        valueNameSize = sizeof(valueName) / sizeof(valueName[0]);
        valueDataSize = sizeof(valueData);

        LONG result = RegEnumValueW(hKey, index, valueName, &valueNameSize, nullptr, &valueType, valueData, &valueDataSize);

        if (result != ERROR_SUCCESS) {
            break;
        }

        // Convert valueData to a char array for comparison
        char valueDataString[1024];
        WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)valueData, -1, valueDataString, sizeof(valueDataString), nullptr, nullptr);

        // Check if any virtual machine strings are present in the registry entry
        for (int i = 0; i < sizeof(virtualMachineStrings) / sizeof(virtualMachineStrings[0]); i++) {
            if (strstr(valueDataString, virtualMachineStrings[i]) != nullptr) {
                return 1;  // Virtual machine string detected
            }
        }
        index++;
    }
    return 0;  // No virtual machine string detected
}

/*
 * Uses EnumWindowsWin to count the top-level windows running on the system. If the count is less than 12, it
 * invokes TerminateProcess. It also checks for the presence of specific windows (such as MFC) and considers the sample
 * to be running in a virtual machine if detected.
 * */


// Callback function for EnumWindows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    int* windowCount = (int*)lParam;
    (*windowCount)++;
    return TRUE;
}

// Function to check if the system is running in a virtual machine
BOOL enumWindows() {
    int windowCount = 0;

    // Enumerate top-level windows
    if (EnumWindows(EnumWindowsProc, (LPARAM)&windowCount)) {
//        printf("Number of top-level windows: %d\n", windowCount);

        // Check if the count is less than 12
        if (windowCount < 12) {
            // Additional checks for specific windows (replace with your own criteria)
            if (FindWindowA("MFC", nullptr) != nullptr) {
                // Detected MFC window, consider it as a virtual machine
                return 1;
            }
            // If conditions are met, terminate the process
            printf("Terminating the process.\n");
            TerminateProcess(GetCurrentProcess(), 0);
            return 1;
        }
    } else {
        // Handle error
        printf("Error enumerating windows.\n");
    }

    return 0;  // Not in a virtual machine
}


/*
 * 	Uses GetModuleHandleA to search for common DLL files associated with sandboxes, such as sbiedll, aswook, snxhk.
 * */

BOOL checkModules() {
    const char* sandboxDLLs[] = { "sbiedll.dll", "aswook.dll", "snxhk.dll" };

    for (int i = 0; i < sizeof(sandboxDLLs) / sizeof(sandboxDLLs[0]); i++) {
        HMODULE hModule = GetModuleHandleA(sandboxDLLs[i]);

        if (hModule != nullptr) {
            // Detected sandbox-related DLL, consider it as a sandbox environment
            return 1;
        }
    }
    return 0;  // Not in a sandbox environment
}

/*
 * Uses isDebuggerPresentApi to test debuggers. If there is no debugger it returns 0.
 * */

BOOL isDebuggerPresentApi () {
    return IsDebuggerPresent();
}

/*
 * Test global flags to detect debuggers.
 * If the 32-bit executable is being run on a 64-bit system, both the 32-bit and 64-bit PEBs are checked. The WoW64 PEB
 * address is fetched via the WoW64 Thread Environment Block (TEB).
 * */

BOOL checkNtGlobalFlag () {
    PDWORD pNtGlobalFlag = nullptr, pNtGlobalFlagWoW64 = nullptr;

    #if defined (ENV64BIT)
    pNtGlobalFlag = (PDWORD)(__readgsqword(0x60) + 0xBC);

    #elif defined(ENV32BIT)
    /* NtGlobalFlags for real 32-bits OS */
	BYTE* _teb32 = (BYTE*)__readfsdword(0x18);
	DWORD _peb32 = *(DWORD*)(_teb32 + 0x30);
	pNtGlobalFlag = (PDWORD)(_peb32 + 0x68);

	if (IsWoW64())
	{
		/* In Wow64, there is a separate PEB for the 32-bit portion and the 64-bit portion
		which we can double-check */

		BYTE* _teb64 = (BYTE*)__readfsdword(0x18) - 0x2000;
		DWORD64 _peb64 = *(DWORD64*)(_teb64 + 0x60);
		pNtGlobalFlagWoW64 = (PDWORD)(_peb64 + 0xBC);
	}
    #endif

    BOOL normalDetected = pNtGlobalFlag && *pNtGlobalFlag & 0x00000070;
    BOOL wow64Detected = pNtGlobalFlagWoW64 && *pNtGlobalFlagWoW64 & 0x00000070;

    if(normalDetected || wow64Detected)
        return TRUE;
    else
        return FALSE;
}

/*
 * Establishes a connection to the Service Control Manager on the computer, opens the specified Service Control Manager
 * database, and uses EnumServicesStatusA to enumerate the services in the Service Control Manager database.
 * */
BOOL isVirtualizationServicePresent() {
    const char* virtualizationServiceNames[] = {
            "VMwareTools",
            "VBoxService",
            "VMUSBArbService",   // VMware USB Arbitration Service
            "VMnetDHCP",         // VMware NAT Service
            "VMAuthdService",    // VMware Authorization Service

            "vboxadd-service",   // VirtualBox Guest Additions Service (Linux)
            "VBoxUSBMon",        // VirtualBox USB Support Service

            "vmicvss",           // Hyper-V Data Exchange Service
            "vmicshutdown",      // Hyper-V Guest Shutdown Service
            "vmicheartbeat",     // Hyper-V Heartbeat Service
    };


    SC_HANDLE scmHandle = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);

    if (scmHandle == nullptr) {
        // Handle error opening SCM
        printf("Error opening SCM.\n");
        return -1;
    }

    DWORD bufferSize = 0;
    DWORD serviceCount = 0;

    // Get the required buffer size
    EnumServicesStatusA(scmHandle, SERVICE_WIN32, SERVICE_STATE_ALL, nullptr, 0, &bufferSize, &serviceCount, nullptr);

    if (GetLastError() != ERROR_MORE_DATA) {
        // Handle error getting buffer size
        CloseServiceHandle(scmHandle);
        printf("Error getting buffer size.\n");
        return -1;
    }

    // Allocate buffer for service information
    ENUM_SERVICE_STATUSA* serviceInfo = (ENUM_SERVICE_STATUSA*)malloc(bufferSize);

    if (serviceInfo == nullptr) {
        // Handle error, unable to allocate memory
        CloseServiceHandle(scmHandle);
        printf("Error allocating memory.\n");
        return -1;
    }

    // Enumerate services
    if (!EnumServicesStatusA(scmHandle, SERVICE_WIN32, SERVICE_STATE_ALL, serviceInfo, bufferSize, &bufferSize, &serviceCount, nullptr)) {
        // Handle error enumerating services
        free(serviceInfo);
        CloseServiceHandle(scmHandle);
        printf("Error enumerating services.\n");
        return -1;
    }

    // Check if virtualization-related services are present
    for (DWORD i = 0; i < serviceCount; i++) {
        for (int j = 0; j < sizeof(virtualizationServiceNames) / sizeof(virtualizationServiceNames[0]); j++) {
            // Check if the service name contains the virtualization-related keywords
            if (strstr(serviceInfo[i].lpServiceName, virtualizationServiceNames[j]) != nullptr) {
                free(serviceInfo);
                CloseServiceHandle(scmHandle);
                return 1;  // Virtualization-related service detected
            }
        }
    }

    free(serviceInfo);
    CloseServiceHandle(scmHandle);
    return 0;  // No virtualization-related service detected
}


/*
 * Uses GetUserNameA to verify the username associated with sandbox software and raises an exception if detected.
 * */

BOOL checkSandboxUsername() {
    char username[UNLEN + 1];
    DWORD usernameSize = sizeof(username);

    // Get the current username
    if (GetUserNameA(username, &usernameSize)) {
        // Check if the username is associated with sandbox software
        const char* sandboxUsernames[] = {
                "sandbox_user",
                "cuckoo",         // Cuckoo Sandbox
                "fireeye",        // Fireeye Sandbox
                "joesandbox",     // Joe Sandbox
                "threatgrid",     // Threat Grid
                "hybrid-analysis", // Hybrid Analysis
                "anyrun"           // Any.Run
        };

        for (int i = 0; i < sizeof(sandboxUsernames) / sizeof(sandboxUsernames[0]); i++) {
            if (strstr(username, sandboxUsernames[i]) != nullptr) {
                // Detected sandbox username
                return true;
            }
        }

        // No sandbox username detected
        return false;
    } else {
        // Handle error getting username
        printf("Error getting username.\n");
        return false;
    }
}

