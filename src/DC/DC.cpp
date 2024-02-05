#include "DC.hpp"
#include "Util.hpp"
#include "ControlCode.h"
#include "DriverDefine.h"
#include "InputStruct.h"

Driver::Driver()
	: m_hDriver(INVALID_HANDLE_VALUE)
{
	WCHAR szCurrentDirectory[MAX_PATH] = { 0 };
	if (!GetCurrentDirectory(MAX_PATH, szCurrentDirectory)) {
		return;
	}
	if (!LoadDriver(DRIVER_SERVICE_NAME, (WSTRING(szCurrentDirectory) + L"\\" + WSTRING(DRIVER_NAME)).c_str())) {
		return;
	}

	m_hDriver = CreateFile(DEVICE_SYMBOL_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return;
	}

	ULONG ulCurrentProcessId = GetCurrentProcessId();
	if (!DeviceIoControl(m_hDriver, IOCTL_SET_TARGET_PROCESS, &ulCurrentProcessId, sizeof(ULONG), NULL, 0, NULL, NULL)) {
		CloseHandle(m_hDriver);
		m_hDriver = INVALID_HANDLE_VALUE;
	}
}

Driver::~Driver() {
	if (m_hDriver != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hDriver);
	}
}

BOOL Driver::Unload() {
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return TRUE;
	}
	BOOL Retval = UnloadDriver(DRIVER_SERVICE_NAME);
	if (Retval) {
		m_hDriver = INVALID_HANDLE_VALUE;
	}
	return Retval;
}

Vector<ProcessInfo> Driver::CollectProcessInfo() {
	Vector<ProcessInfo> ProcessInfoVector = *new Vector<ProcessInfo>;
	PPROCESS_INFO* pProcessInfoList = NULL;
	DWORD dwCount = 0;

	if (m_hDriver == INVALID_HANDLE_VALUE) {
		goto Return;
	}

	if (!DeviceIoControl(m_hDriver, IOCTL_COLLECT_PROCESS_INFO, NULL, 0, &pProcessInfoList, sizeof(PPROCESS_INFO*), &dwCount, NULL)) {
		goto Return;
	}
	if (!pProcessInfoList) {
		goto Return;
	}

	for (ULONG i = 0; i < dwCount; i++) {
		ProcessInfoVector.push_back(*(new ProcessInfo(pProcessInfoList[i])));
	}

Return:
	return ProcessInfoVector;
}

Vector<ObjectTypeInfo> Driver::CollectObjectTypeInfo() {
	Vector<ObjectTypeInfo> ObjectInfoVector = *new Vector<ObjectTypeInfo>;
	POBJECT_TYPE_INFO* pObjectInfoList = NULL;
	DWORD dwCount = 0;

	if (m_hDriver == INVALID_HANDLE_VALUE) {
		goto Return;
	}

	if (!DeviceIoControl(m_hDriver, IOCTL_COLLECT_OBJECT_TYPE_INFO, NULL, 0, &pObjectInfoList, sizeof(POBJECT_TYPE_INFO*), &dwCount, NULL)) {
		goto Return;
	}
	if (!pObjectInfoList) {
		goto Return;
	}

	for (ULONG i = 0; i < dwCount; i++) {
		ObjectInfoVector.push_back(*(new ObjectTypeInfo(pObjectInfoList[i])));
	}

Return:
	return ObjectInfoVector;
}

ProcessDetailInfo Driver::CollectProcessDetailInfo(PVOID pEprocess) {
	PPROCESS_DETAIL pProcessDetail = NULL;

	if (m_hDriver == INVALID_HANDLE_VALUE || !pEprocess) {
		goto Error;
	}

	if (!DeviceIoControl(m_hDriver, IOCTL_COLLECT_PROCESS_DETAIL, &pEprocess, sizeof(PVOID), &pProcessDetail, sizeof(PPROCESS_DETAIL), NULL, NULL)) {
		goto Error;
	}
	if (!pProcessDetail) {
		goto Error;
	}

	return ProcessDetailInfo(pProcessDetail);
Error:
	return ProcessDetailInfo();
}

ObjectTypeDetail Driver::CollectObjectTypeDetail(PVOID pObjectType) {
	POBJECT_TYPE_DETAIL pDetail = NULL;

	if (m_hDriver == INVALID_HANDLE_VALUE || !pObjectType) {
		goto Error;
	}

	if (!DeviceIoControl(m_hDriver, IOCTL_COLLECT_OBJECT_TYPE_DETAIL, &pObjectType, sizeof(PVOID), &pDetail, sizeof(POBJECT_TYPE_DETAIL), NULL, NULL)) {
		goto Error;
	}
	if (!pDetail) {
		goto Error;
	}

	return ObjectTypeDetail(pDetail);
Error:
	return ObjectTypeDetail();
}

PVOID Driver::MdlReadMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	ULONG ulReadLength
) {
	if (!pEprocess || !VirtualAddress || !ulReadLength) {
		return NULL;
	}
	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	ReadMemoryInfo.pEprocess = pEprocess;
	ReadMemoryInfo.VirtualAddress = VirtualAddress;
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_MDL_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID Driver::MdlWriteMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	PVOID pBuffer,
	ULONG ulWriteLength
) {
	if (!pEprocess || !pBuffer || !ulWriteLength) {
		return NULL;
	}
	ULONG ulStructSize = sizeof(WRITE_MEMORY_INFO) + ulWriteLength;
	PWRITE_MEMORY_INFO pWriteMemoryInfo = (PWRITE_MEMORY_INFO)AllocateZeroedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		return NULL;
	}
	pWriteMemoryInfo->pEprocess = pEprocess;
	pWriteMemoryInfo->VirtualAddress = VirtualAddress;
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_MDL_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

PVOID Driver::PhysicalReadMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	ULONG ulReadLength
) {
	if (!pEprocess || !VirtualAddress || !ulReadLength) {
		return NULL;
	}
	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	ReadMemoryInfo.pEprocess = pEprocess;
	ReadMemoryInfo.VirtualAddress = VirtualAddress;
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_PHYSICAL_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID Driver::PhysicalWriteMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	PVOID pBuffer,
	ULONG ulWriteLength
) {
	if (!pEprocess || !pBuffer || !ulWriteLength) {
		return NULL;
	}
	ULONG ulStructSize = sizeof(WRITE_MEMORY_INFO) + ulWriteLength;
	PWRITE_MEMORY_INFO pWriteMemoryInfo = (PWRITE_MEMORY_INFO)AllocateZeroedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		return NULL;
	}
	pWriteMemoryInfo->pEprocess = pEprocess;
	pWriteMemoryInfo->VirtualAddress = VirtualAddress;
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_PHYSICAL_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

PVOID Driver::Cr3ReadMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	ULONG ulReadLength
) {
	if (!pEprocess || !VirtualAddress || !ulReadLength) {
		return NULL;
	}
	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	ReadMemoryInfo.pEprocess = pEprocess;
	ReadMemoryInfo.VirtualAddress = VirtualAddress;
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_CR3_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID Driver::Cr3WriteMemory(
	PVOID pEprocess,
	PVOID VirtualAddress,
	PVOID pBuffer,
	ULONG ulWriteLength
) {
	if (!pEprocess || !pBuffer || !ulWriteLength) {
		return NULL;
	}
	ULONG ulStructSize = sizeof(WRITE_MEMORY_INFO) + ulWriteLength;
	PWRITE_MEMORY_INFO pWriteMemoryInfo = (PWRITE_MEMORY_INFO)AllocateZeroedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		return NULL;
	}
	pWriteMemoryInfo->pEprocess = pEprocess;
	pWriteMemoryInfo->VirtualAddress = VirtualAddress;
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, IOCTL_CR3_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

BOOL Driver::TerminateProcess(PVOID pEprocess) {
	if (m_hDriver == INVALID_HANDLE_VALUE || !pEprocess) {
		return FALSE;
	}

	return DeviceIoControl(m_hDriver, IOCTL_TERMINATE_PROCESS, &pEprocess, sizeof(PVOID), NULL, 0, NULL, NULL);
}

