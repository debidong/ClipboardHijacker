#ifndef CLIPBOARD_ANTIDETECTION_H
#define CLIPBOARD_ANTIDETECTION_H

#include <windows.h>
#include <winnt.h>
#include <cstdio>
#include <Lmcons.h>

#endif //CLIPBOARD_ANTIDETECTION_H

BOOL checkReg();
BOOL enumWindows();
BOOL checkModules();
BOOL isDebuggerPresentApi ();
BOOL checkNtGlobalFlag ();
BOOL isVirtualizationServicePresent();
BOOL checkSandboxUsername();