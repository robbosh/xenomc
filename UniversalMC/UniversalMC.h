#include "NtDefinitions.h"
#include "Utils.h"
#include "resource.h"

void CloseRemoteMutex(SYSTEM_HANDLE sh);
bool CheckIfHandleIsTibiaMutex(SYSTEM_HANDLE sh);
void CloseTibiaMutex(DWORD tibiaPID);
BOOL CALLBACK CloseMutexCallback(HWND temp, LPARAM lParam);