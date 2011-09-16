#ifndef	_SYSTEM_HANDLE_H
#define _SYSTEM_HANDLE_H

#include <windows.h>
#include <shellapi.h>
#include <list>

#include "NtDefinitions.h"
#include "Utils.h"

using namespace std;

class SystemHandle
{
public:
	static list<SystemHandle*> EnumerateProcessHandles(DWORD procID);

	SystemHandle(SYSTEM_HANDLE handle);
	SystemHandle::~SystemHandle();

	SYSTEM_HANDLE GetNativeSystemHandle();
	HANDLE GetNativeHandle();
	char* GetName();

	void Close(bool asMutex);

protected:
	SYSTEM_HANDLE systemHandle;
	HANDLE process;
	char handleName[MAX_PATH];

private:
	void GetHandleName();
};

#endif
