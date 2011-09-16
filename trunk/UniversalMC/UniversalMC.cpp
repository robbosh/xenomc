//XenoMC by DarkstaR.
//Special thanks to the authors of the following pages:
//http://forum.sysinternals.com/topic14546.html
//http://www.codeproject.com/KB/shell/OpenedFileFinder.aspx


#include <windows.h>
#include <shellapi.h>
#include <list>
#include <string>
#include <iostream>
#include <shellapi.h>

#include "resource.h"

#include "SystemHandle.h"
#include "Utils.h"

using namespace std;

void CloseTibiaMutex(DWORD tibiaPID)
{
	static list<DWORD> finishedClients;
	list<DWORD>::iterator C;
	for (C = finishedClients.begin(); C != finishedClients.end(); C++) //C++ hahaha total accident
	{
		if ((DWORD)(*C) == tibiaPID)
			return;
	}

	list<SystemHandle*> handles = SystemHandle::EnumerateProcessHandles(tibiaPID);
	list<SystemHandle*>::iterator hit;
	SystemHandle* sysHandle;

	for (hit = handles.begin(); hit != handles.end(); hit++)
	{
		sysHandle = (SystemHandle*)*hit;
		if (strcmp(sysHandle->GetName(), "\\Sessions\\1\\BaseNamedObjects\\TibiaPlayerMutex") == 0)
		{
			sysHandle->Close(true);
			finishedClients.push_back(sysHandle->GetNativeSystemHandle().dwProcessId);
			cout << "Closed mutex for Tibia client with PID " << sysHandle->GetNativeSystemHandle().dwProcessId << endl;
		}

		delete sysHandle;
	}

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
		cout << "This program is safe and makes no modifications to the Tibia client!" << endl;
		cout << "Leave it running and open as many Tibias as you would like." << endl;
		cout << "Open your clients slowly! Give this program time to react." << endl;

		SetConsoleTitle(L"XenoMC");

		while (true)
		{
			EnumWindows(CloseMutexCallback, NULL); //Enumerate for "Tibia" windows and close their mutexs
			Sleep(200);
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

