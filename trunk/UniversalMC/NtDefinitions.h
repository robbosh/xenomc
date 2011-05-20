#include <windows.h>
#include <ntstatus.h>


#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef ULONG (NTAPI *_NtDuplicateObject)(
	 HANDLE SourceProcessHandle,
	 HANDLE SourceHandle,
	 HANDLE TargetProcessHandle,
	 PHANDLE TargetHandle,
	 ACCESS_MASK DesiredAccess,
	 ULONG Attributes,
	 ULONG Options
 );

typedef NTSTATUS (NTAPI *_NtClose)(
	 HANDLE Handle
 );

typedef NTSTATUS (NTAPI *_NtQuerySystemInformation)(
  ULONG SystemInformationClass,
  PVOID SystemInformation,
  ULONG SystemInformationLength,
  PULONG ReturnLength
);

typedef NTSTATUS (NTAPI *_NtQueryObject)(
    HANDLE Handle,
	ULONG ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength

);

typedef struct _HANDLE_INFO
{	
	USHORT tcDeviceName[260];
	USHORT tcFileName[260];
    ULONG uType;
}HANDLE_INFO;

typedef struct _ADDRESS_INFO
{
	PVOID pAddress;
}ADDRESS_INFO;

typedef LONG NTSTATUS;

typedef struct _SYSTEM_HANDLE
{
    DWORD       dwProcessId;
    BYTE		bObjectType;
    BYTE		bFlags;
    WORD		wValue;
	PVOID       pAddress;
	DWORD GrantedAccess;
}SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    DWORD         dwCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION, **PPSYSTEM_HANDLE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemHandleInformation = 0x10,
} SYSTEM_INFORMATION_CLASS;

typedef enum _OBJECT_INFORMATION_CLASS {
    ObjectBasicInformation = 0x0,
	ObjectNameInformation = 0x1,
	ObjectTypeInformation = 0x2
} _OBJECT_INFORMATION_CLASS;

typedef struct _UNICODE_STRING {
        USHORT  Length;
        USHORT  MaximumLength;
        PWSTR   Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS
} POOL_TYPE, *PPOOL_TYPE;

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING  Name;
    ULONG           TotalNumberOfObjects;
    ULONG           TotalNumberOfHandles;
    ULONG           TotalPagedPoolUsage;
    ULONG           TotalNonPagedPoolUsage;
    ULONG           TotalNamePoolUsage;
    ULONG           TotalHandleTableUsage;
    ULONG           HighWaterNumberOfObjects;
    ULONG           HighWaterNumberOfHandles;
    ULONG           HighWaterPagedPoolUsage;
    ULONG           HighWaterNonPagedPoolUsage;
    ULONG           HighWaterNamePoolUsage;
    ULONG           HighWaterHandleTableUsage;
    ULONG           InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG           ValidAccess;
    BOOLEAN         SecurityRequired;
    BOOLEAN         MaintainHandleCount;
    USHORT          MaintainTypeList;
    POOL_TYPE       PoolType;
    ULONG           PagedPoolUsage;
    ULONG           NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

#define IOCTL_LISTDRV_BUFFERED_IO		\
        CTL_CODE(FILE_DEVICE_UNKNOWN,	\
        0x802,							\
        METHOD_BUFFERED,	            \
        FILE_READ_DATA | FILE_WRITE_DATA)


typedef NTSTATUS ( WINAPI *PNtQuerySystemInformation)
				 ( IN	SYSTEM_INFORMATION_CLASS SystemInformationClass,
				   OUT	PVOID					 SystemInformation,
				   IN	ULONG					 SystemInformationLength,
				   OUT	PULONG					 ReturnLength OPTIONAL );

_NtDuplicateObject NtDuplicateObject;
_NtClose NtClose;
_NtQuerySystemInformation NtQuerySystemInformation;
_NtQueryObject NtQueryObject;

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