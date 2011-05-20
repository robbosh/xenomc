//XenoMC by DarkstaR.
//Special thanks to the authors of the following pages:
//http://forum.sysinternals.com/topic14546.html
//http://www.codeproject.com/KB/shell/OpenedFileFinder.aspx

#include <string>
#include <iostream>
#include "UniversalMC.h"

using namespace std;


void CloseRemoteMutex(SYSTEM_HANDLE sh)
{
	HANDLE hProcess, hFake;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, sh.dwProcessId);
	if (NT_SUCCESS(NtDuplicateObject(hProcess, (HANDLE)sh.wValue, GetCurrentProcess(), &hFake, 0, 0, DUPLICATE_CLOSE_SOURCE)))
	{
		ReleaseMutex(hFake);
		CloseHandle(hFake);
	}

	NtClose(hProcess);
}

bool CheckIfHandleIsTibiaMutex(SYSTEM_HANDLE sh)
{
	//This function can be cleaned up alot. But I'm lazy. :D
	HANDLE hProcess, hFake;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, sh.dwProcessId);
	if (NT_SUCCESS(NtDuplicateObject(hProcess, (HANDLE)sh.wValue, GetCurrentProcess(), &hFake, 0, 0, 0)))
	{
		POBJECT_TYPE_INFORMATION	typeInfo = (POBJECT_TYPE_INFORMATION)new BYTE[0x1000];
		PUNICODE_STRING				nameInfo = (PUNICODE_STRING)new BYTE[0x1000];
		DWORD						read;
		
		
		if (NT_SUCCESS(NtQueryObject(hFake, ObjectTypeInformation, typeInfo, 0x1000, &read)))
		{

			if (!NT_SUCCESS(NtQueryObject(hFake, ObjectNameInformation, nameInfo, 0x1000, &read)))
			{
				//We need a bigger name buffer
				delete nameInfo;
				nameInfo = (PUNICODE_STRING)new BYTE[0x1000];
				if (!NtQueryObject(hFake, ObjectNameInformation, nameInfo, 0x1000, &read))
				{
					//Cleanup, no name info
					CloseHandle((HANDLE)sh.wValue);
					CloseHandle(hProcess);
					delete nameInfo;
					delete typeInfo;
					return false;
				}
			}

			if (nameInfo->Length > 0)
			{
				char* objectName = new char[nameInfo->Length];
				WideToChar(objectName, nameInfo->Buffer);

				if (strcmp(objectName, "\\Sessions\\1\\BaseNamedObjects\\TibiaPlayerMutex") == 0)
				{
					//Cleanup and return true, this is our mutex
					ReleaseMutex(hFake);
					CloseHandle(hFake);
					CloseHandle(hProcess);
					delete nameInfo;
					delete typeInfo;
					return true;
				}
			}
		}
		else
		{
			//Cleanup, no type info
			CloseHandle(hFake);
			CloseHandle(hProcess);
			delete nameInfo;
			delete typeInfo;
			return false;
		}

		delete nameInfo;
		delete typeInfo;
	}

	CloseHandle(hProcess);
	return false;
}

void CloseTibiaMutex(DWORD tibiaPID)
{
	PSYSTEM_HANDLE_INFORMATION pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[0x1000];
	DWORD needed = 0;
	
	if(!NT_SUCCESS(NtQuerySystemInformation(SystemHandleInformation, pSysHandleInformation, 0x1000, &needed)))
	{
		//We need a larger buffer

		delete pSysHandleInformation;
		pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[needed];
		if(!NT_SUCCESS(NtQuerySystemInformation(SystemHandleInformation, pSysHandleInformation, needed, &needed)))
		{
			//clean up, no handle info
			delete pSysHandleInformation;
			return;
		}
	}

	for (int i = 0; i < pSysHandleInformation->dwCount; i++)
	{
		if (pSysHandleInformation->Handles[i].dwProcessId == tibiaPID)
		{
			if (CheckIfHandleIsTibiaMutex(pSysHandleInformation->Handles[i]))
			{
				CloseRemoteMutex(pSysHandleInformation->Handles[i]);
				cout << "Closed mutex for Tibia client with PID " << pSysHandleInformation->Handles[i].dwProcessId << endl;
			}
		}
	}

	delete pSysHandleInformation;
}

BOOL CALLBACK CloseMutexCallback(HWND temp, LPARAM lParam)
{
	const char* wndText = new char[12];
	GetClassNameA(temp, (LPSTR)wndText, 12);
	if (strcmp(wndText, "TibiaClient") == 0)
	{
		DWORD PID;
		if (GetWindowThreadProcessId(temp, &PID) != 0)
			CloseTibiaMutex(PID); //Its a Tibia window, close the mutex
	}

	delete [] wndText;
	return TRUE;
}


int main()
{
	if (NtLoadFunctions())
	{
		cout << "Keep this program running and open as many Tibias as you would like." << endl;
		cout << "Open the clients slowly. Opening them too quick will cause this to not work." << endl;
		while (true)
		{
			EnumWindows(CloseMutexCallback, NULL); //Enumerate for "Tibia" windows and close their mutexs
			Sleep(400);
		}
	}
	else
	{
		cout << "ERROR: Couldn't load NT functions." << endl;
		while (true)
			Sleep(500);
	}

	return 0;
}

