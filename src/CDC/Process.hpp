#pragma once
#include <Windows.h>
#include <string>
#include "R3S_Define.h"
#include "PS_CD_OutputStruct.h"
#include "R3S_TypeDefine.h"

class DLL_EXPORT ProcessInfo {
public:
	PVOID m_pEprocess;
	PVOID m_ProcessId;
	LARGE_INTEGER m_CreateTime;
	ULONGLONG m_ullOwnerProcessId;
	WSTRING m_szFullFileName;

public:
	ProcessInfo() = default;
	ProcessInfo(PPROCESS_INFO pProcessInfo);
	~ProcessInfo() {}

};

class DLL_EXPORT ProcessDetailPeb {
public:
	PVOID m_pPeb;
	UCHAR m_ucInheritedAddressSpace;
	UCHAR m_ucBeingDebugged;
	PVOID m_pImageBaseAddress;
	PVOID m_pProcessHeap;
	ULONG m_ulSystemReserved;
	ULONG m_ulNumberOfProcessors;
	ULONG m_ulNumberOfHeaps;
	ULONG m_ulMaximumNumberOfHeaps;
	ULONG m_ulSessionId;
public:
	ProcessDetailPeb() = default;
	ProcessDetailPeb(PPROCESS_DETAIL_PEB pPebDetail);
	~ProcessDetailPeb() {}
};

class DLL_EXPORT ProcessDetailPcb {
public:
	PVOID m_pPcb;
	ULONGLONG m_ullDirectoryTableBase;
	ULONG m_ulKernelTime;
	ULONG m_ulUserTime;
	ULONG m_ulReadyTime;
	ULONGLONG m_ullUserDirectoryTableBase;
	ULONGLONG m_ullKernelWaitTime;
	ULONGLONG m_ullUserWaitTime;
public:
	ProcessDetailPcb() = default;
	ProcessDetailPcb(PPROCESS_DETAIL_PCB pPcbDetail);
	~ProcessDetailPcb() {}
};

class DLL_EXPORT ProcessDetailEprocess {
public:
	PVOID m_pEprocess;
	PVOID m_UniqueProcessId;
	LARGE_INTEGER m_CreateTime;
	ULONGLONG m_ullPeakVirtualSize;
	ULONGLONG m_ullVirtualSize;
	ULONGLONG m_ullMmReserved;
	ULONG m_ulCookie;
	ULONGLONG m_ullOwnerProcessId;
	PVOID m_pDebugPort;
	UCHAR m_ucPriorityClass;
	PVOID m_pSecurityPort;
	PVOID m_VadHint;
	ULONGLONG m_ullVadCount;
	ULONGLONG m_ullVadPhysicalPages;
	ULONGLONG m_ullVadPhysicalPagesLimit;
	ULONGLONG m_ullSecurityDomain;
	ULONGLONG m_ullParentSecurityDomain;
public:
	ProcessDetailEprocess() = default;
	ProcessDetailEprocess(PPROCESS_DETAIL_EPROCESS pEprocessDetail);
	~ProcessDetailEprocess() {}
};

class DLL_EXPORT ProcessDetailInfo {
public:
	ProcessDetailEprocess m_Eprocess;
	ProcessDetailPcb m_Pcb;
	ProcessDetailPeb m_Peb;
public:
	ProcessDetailInfo() = default;
	ProcessDetailInfo(PPROCESS_DETAIL pProcessDetail);
	~ProcessDetailInfo() {}

};
