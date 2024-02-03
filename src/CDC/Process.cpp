#include "Process.hpp"
#include "Collector.hpp"
#include "PS_CD_ControlCode.h"
#include "R3S_Util.h"

ProcessInfo::ProcessInfo(PPROCESS_INFO pProcessInfo) {
	if (!pProcessInfo) {
		return;
	}
	m_pEprocess = pProcessInfo->pEprocess;
	m_ProcessId = pProcessInfo->ProcessId;
	m_ullOwnerProcessId = pProcessInfo->ullOwnerProcessId;
	m_CreateTime = pProcessInfo->CreateTime;
	if (pProcessInfo->pFullFileName) {
		m_szFullFileName = WSTRING(pProcessInfo->pFullFileName);
		FreeMemory(pProcessInfo->pFullFileName);
	}
	FreeMemory(pProcessInfo);
}

ProcessDetailPeb::ProcessDetailPeb(PPROCESS_DETAIL_PEB pPebDetail) {
	if (!pPebDetail) {
		return;
	}

	m_pPeb = pPebDetail->pPeb;
	m_ucInheritedAddressSpace = pPebDetail->ucInheritedAddressSpace;
	m_ucBeingDebugged = pPebDetail->ucBeingDebugged;
	m_pImageBaseAddress = pPebDetail->pImageBaseAddress;
	m_pProcessHeap = pPebDetail->pProcessHeap;
	m_ulSystemReserved = pPebDetail->ulSystemReserved;
	m_ulNumberOfProcessors = pPebDetail->ulNumberOfProcessors;
	m_ulNumberOfHeaps = pPebDetail->ulNumberOfHeaps;
	m_ulMaximumNumberOfHeaps = pPebDetail->ulMaximumNumberOfHeaps;
	m_ulSessionId = pPebDetail->ulSessionId;
}

ProcessDetailPcb::ProcessDetailPcb(PPROCESS_DETAIL_PCB pPcbDetail) {
	if (!pPcbDetail) {
		return;
	}

	m_pPcb = pPcbDetail->pPcb;
	m_ullDirectoryTableBase = pPcbDetail->ullDirectoryTableBase;
	m_ulKernelTime = pPcbDetail->ulKernelTime;
	m_ulUserTime = pPcbDetail->ulUserTime;
	m_ulReadyTime = pPcbDetail->ulReadyTime;
	m_ullUserDirectoryTableBase = pPcbDetail->ullUserDirectoryTableBase;
	m_ullKernelWaitTime = pPcbDetail->ullKernelWaitTime;
	m_ullUserWaitTime = pPcbDetail->ullUserWaitTime;
}

ProcessDetailEprocess::ProcessDetailEprocess(PPROCESS_DETAIL_EPROCESS pEprocessDetail) {
	if (!pEprocessDetail) {
		return;
	}

	m_pEprocess = pEprocessDetail->pEprocess;
	m_UniqueProcessId = pEprocessDetail->UniqueProcessId;
	m_CreateTime = pEprocessDetail->CreateTime;
	m_ullPeakVirtualSize = pEprocessDetail->ullPeakVirtualSize;
	m_ullVirtualSize = pEprocessDetail->ullVirtualSize;
	m_ullMmReserved = pEprocessDetail->ullMmReserved;
	m_ulCookie = pEprocessDetail->ulCookie;
	m_ullOwnerProcessId = pEprocessDetail->ullOwnerProcessId;
	m_pDebugPort = pEprocessDetail->pDebugPort;
	m_ucPriorityClass = pEprocessDetail->ucPriorityClass;
	m_pSecurityPort = pEprocessDetail->pSecurityPort;
	m_VadHint = pEprocessDetail->VadHint;
	m_ullVadCount = pEprocessDetail->ullVadCount;
	m_ullVadPhysicalPages = pEprocessDetail->ullVadPhysicalPages;
	m_ullVadPhysicalPagesLimit = pEprocessDetail->ullVadPhysicalPagesLimit;
	m_ullSecurityDomain = pEprocessDetail->ullSecurityDomain;
	m_ullParentSecurityDomain = pEprocessDetail->ullParentSecurityDomain;
}

ProcessDetailInfo::ProcessDetailInfo(PPROCESS_DETAIL pProcessDetail) {
	if (!pProcessDetail) {
		return;
	}

	m_Eprocess = ProcessDetailEprocess(&(pProcessDetail->Eprocess));
	m_Pcb = ProcessDetailPcb(&(pProcessDetail->Pcb));
	m_Peb = ProcessDetailPeb(&(pProcessDetail->Peb));
}