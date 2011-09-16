#include "SystemHandle.h"
SystemHandle::SystemHandle(SYSTEM_HANDLE handle)
{
	this->systemHandle = handle;
	this->process = OpenProcess(PROCESS_ALL_ACCESS, 0, this->systemHandle.dwProcessId);
	GetHandleName();
}
SystemHandle::~SystemHandle()
{
	if (this->process) CloseHandle(this->process);
}

SYSTEM_HANDLE SystemHandle::GetNativeSystemHandle() { return this->systemHandle; }
HANDLE SystemHandle::GetNativeHandle() { return (HANDLE)this->systemHandle.wValue; }
char* SystemHandle::GetName() { return this->handleName; }

void SystemHandle::Close(bool asMutex)
{
	HANDLE hFake;
	if (NT_SUCCESS(NtDuplicateObject(this->process, this->GetNativeHandle(), GetCurrentProcess(), &hFake, 0, 0, DUPLICATE_CLOSE_SOURCE)))
	{
		if (asMutex) ReleaseMutex(hFake);
		NtClose(hFake);
	}
}

void SystemHandle::GetHandleName()
{
	HANDLE hFake;
	char* objectName = NULL;
	if (NT_SUCCESS(NtDuplicateObject(this->process, this->GetNativeHandle(), GetCurrentProcess(), &hFake, 0, 0, 0)))
	{
		POBJECT_TYPE_INFORMATION typeInfo = (POBJECT_TYPE_INFORMATION)new BYTE[0x1000];
		PUNICODE_STRING nameInfo = (PUNICODE_STRING)new BYTE[0x1000];
		DWORD read;
		if (NT_SUCCESS(NtQueryObject(hFake, ObjectTypeInformation, typeInfo, 0x1000, &read)))
		{
			if (!NT_SUCCESS(NtQueryObject(hFake, ObjectNameInformation, nameInfo, 0x1000, &read)))
			{	//We need a bigger name buffer
				delete nameInfo;
				nameInfo = (PUNICODE_STRING)new BYTE[0x1000];
				if (NtQueryObject(hFake, ObjectNameInformation, nameInfo, 0x1000, &read))
				{
					objectName = new char[nameInfo->Length];
					WideToChar(objectName, nameInfo->Buffer);
					strcpy(this->handleName, objectName);
				}
			}
			else if (nameInfo->Length > 0)
			{
				objectName = new char[nameInfo->Length];
				WideToChar(objectName, nameInfo->Buffer);
				strcpy(this->handleName, objectName);
			}
		}
		
		delete nameInfo;
		delete typeInfo;
	}

	if (hFake) CloseHandle(hFake);
}


list<SystemHandle*> SystemHandle::EnumerateProcessHandles(DWORD procID)
{
	list<SystemHandle*> ret;

	PSYSTEM_HANDLE_INFORMATION pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[0x1000];
	DWORD needed = 0;
	
	if(!NT_SUCCESS(NtQuerySystemInformation(SystemHandleInformation, pSysHandleInformation, 0x1000, &needed)))
	{	//We need a larger buffer
		delete pSysHandleInformation;
		pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[needed];
		
		if(NT_SUCCESS(NtQuerySystemInformation(SystemHandleInformation, pSysHandleInformation, needed, &needed)))
			for (int i = 0; i < pSysHandleInformation->dwCount; i++)
				if (pSysHandleInformation->Handles[i].dwProcessId == procID)
					ret.push_back(new SystemHandle(pSysHandleInformation->Handles[i]));
	}
	else
		for (int i = 0; i < pSysHandleInformation->dwCount; i++)
			if (pSysHandleInformation->Handles[i].dwProcessId == procID)
				ret.push_back(new SystemHandle(pSysHandleInformation->Handles[i]));

	delete pSysHandleInformation;
	return ret;
}
