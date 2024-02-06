#pragma once

#ifdef RING0
#include <ntifs.h>
#else
#include <windows.h>
#endif // RING0

#ifdef RING3
#define IRP_MJ_MAXIMUM_FUNCTION 0x1b
#endif // RING3

#pragma pack(8)

typedef struct _PROCESS_INFO {
	PVOID pEprocess;
	PVOID ProcessId;
	LARGE_INTEGER CreateTime;
	ULONGLONG ullOwnerProcessId;
	PWCHAR pFullFileName;
}PROCESS_INFO, * PPROCESS_INFO;

typedef struct _DRIVER_INFO {
	PVOID pDriverBase;
	PVOID pEntryPoint;
	ULONG ulSizeOfImage;
	PVOID MajorFunction[IRP_MJ_MAXIMUM_FUNCTION];
	PWCHAR FullDllName;
}DRIVER_INFO, * PDRIVER_INFO;

typedef struct _OBJECT_TYPE_INFO {
	PVOID pObjectType;
	PVOID pDefaultObject;
	UCHAR ucIndex;
	ULONG ulTotalNumberOfObjects;
	ULONG ulTotalNumberOfHandles;
	PWCHAR szName;
}OBJECT_TYPE_INFO, * POBJECT_TYPE_INFO;

typedef struct _PROCESS_DETAIL_PCB {
	PVOID pPcb;
	ULONGLONG ullDirectoryTableBase;
	ULONG ulKernelTime;
	ULONG ulUserTime;
	ULONG ulReadyTime;
	ULONGLONG ullUserDirectoryTableBase;
	ULONGLONG ullKernelWaitTime;
	ULONGLONG ullUserWaitTime;
}PROCESS_DETAIL_PCB, * PPROCESS_DETAIL_PCB;

typedef struct _PROCESS_DETAIL_PEB {
	PVOID pPeb;
	UCHAR ucInheritedAddressSpace;
	UCHAR ucBeingDebugged;
	PVOID pImageBaseAddress;
	PVOID pProcessHeap;
	ULONG ulSystemReserved;
	ULONG ulNumberOfProcessors;
	ULONG ulNumberOfHeaps;
	ULONG ulMaximumNumberOfHeaps;
	ULONG ulSessionId;
}PROCESS_DETAIL_PEB, * PPROCESS_DETAIL_PEB;

typedef struct _PROCESS_DETAIL_EPROCESS {
	PVOID pEprocess;
	PVOID UniqueProcessId;
	LARGE_INTEGER CreateTime;
	ULONGLONG ullPeakVirtualSize;
	ULONGLONG ullVirtualSize;
	ULONGLONG ullMmReserved;
	ULONG ulCookie;
	ULONGLONG ullOwnerProcessId;
	PVOID pDebugPort;
	UCHAR ucPriorityClass;
	PVOID pSecurityPort;
	PVOID VadHint;
	ULONGLONG ullVadCount;
	ULONGLONG ullVadPhysicalPages;
	ULONGLONG ullVadPhysicalPagesLimit;
	ULONGLONG ullSecurityDomain;
	ULONGLONG ullParentSecurityDomain;
}PROCESS_DETAIL_EPROCESS, * PPROCESS_DETAIL_EPROCESS;

typedef struct _PROCESS_DETAIL {
	PROCESS_DETAIL_EPROCESS Eprocess;
	PROCESS_DETAIL_PCB Pcb;
	PROCESS_DETAIL_PEB Peb;
}PROCESS_DETAIL, * PPROCESS_DETAIL;

typedef struct _OBJECT_TYPE_DETAIL {
	PVOID pObjectType;
	USHORT usObjectTypeFlags;
	ULONG ulObjectTypeCode;
	ULONG ulInvalidAttributes;
	ULONG ulValidAccessMask;
	ULONG ulRetainAccess;
	ULONG ulPoolType;
	ULONG ulDefaultPagedPoolCharge;
	ULONG ulDefaultNonPagedPoolCharge;
	PVOID pDumpProcedure;
	PVOID pOpenProcedure;
	PVOID pCloseProcedure;
	PVOID pDeleteProcedure;
	PVOID pParseProcedure;
	PVOID pParseProcedureEx;
	PVOID pSecurityProcedure;
	PVOID pQueryNameProcedure;
	PVOID pOkayToCloseProcedure;
	ULONG ulWaitObjectFlagMask;
	USHORT usWaitObjectFlagOffset;
	USHORT usWaitObjectPointerOffset;
}OBJECT_TYPE_DETAIL, * POBJECT_TYPE_DETAIL;

typedef struct _MODULE_INFO {
	PWCHAR pFullDllName;
	PWCHAR pBaseDllName;
	PVOID pDllBase;   
	PVOID pEntryPoint;
	ULONG ulSizeOfImage;
	USHORT usLoadCount;
}MODULE_INFO, * PMODULE_INFO;

#pragma pack()