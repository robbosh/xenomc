#include "NtDefinitions.h"

_NtDuplicateObject NtDuplicateObject = NULL;
_NtClose NtClose = NULL;
_NtQuerySystemInformation NtQuerySystemInformation = NULL;
_NtQueryObject NtQueryObject = NULL;

bool NtLoadFunctions()
{
	HINSTANCE NTdll = LoadLibraryA("ntdll.dll");
	if (NTdll)
	{
		NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(NTdll, "NtDuplicateObject");
		NtClose = (_NtClose)GetProcAddress(NTdll, "NtClose");
		NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(NTdll, "NtQuerySystemInformation");
		NtQueryObject = (_NtQueryObject)GetProcAddress(NTdll, "NtQueryObject");

		return true;
	}

	return false;
}