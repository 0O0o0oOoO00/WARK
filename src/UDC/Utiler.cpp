#include "Utiler.hpp"
#include "PS_UD_ControlCode.h"
#include "PS_UD_DriverDefine.h"
#include "PS_UD_InputStruct.h"
#include "R3S_Util.h"
#include "R3S_Driver.h"

UtilerDriver::UtilerDriver()
	: m_hDriver(INVALID_HANDLE_VALUE)
{
	WCHAR szCurrentDirectory[MAX_PATH] = { 0 };
	if (!GetCurrentDirectory(MAX_PATH, szCurrentDirectory)) {
		return;
	}
	if (!LoadDriver(UD_DRIVER_SERVICE_NAME, (WSTRING(szCurrentDirectory) + L"\\" + WSTRING(UD_DRIVER_NAME)).c_str())) {
		return;
	}

	m_hDriver = CreateFile(UD_DEVICE_SYMBOL_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return;
	}

	ULONG ulCurrentProcessId = GetCurrentProcessId();
	if (!DeviceIoControl(m_hDriver, UD_IOCTL_SET_TARGET_PROCESS, &ulCurrentProcessId, sizeof(ULONG), NULL, 0, NULL, NULL)) {
		CloseHandle(m_hDriver);
		m_hDriver = INVALID_HANDLE_VALUE;
	}
}

UtilerDriver::~UtilerDriver() {
	if (m_hDriver != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hDriver);
	}
}

BOOL UtilerDriver::Unload() {
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return TRUE;
	}
	BOOL Retval = UnloadDriver(UD_DRIVER_SERVICE_NAME);
	if (Retval) {
		m_hDriver = INVALID_HANDLE_VALUE;
	}
	return Retval;
}

PVOID UtilerDriver::MdlReadMemory(
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
	DeviceIoControl(m_hDriver, UD_IOCTL_MDL_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID UtilerDriver::MdlWriteMemory(
	PVOID pEprocess,
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
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, UD_IOCTL_MDL_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

PVOID UtilerDriver::PhysicalReadMemory(
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
	DeviceIoControl(m_hDriver, UD_IOCTL_PHYSICAL_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID UtilerDriver::PhysicalWriteMemory(
	PVOID pEprocess,
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
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, UD_IOCTL_PHYSICAL_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

PVOID UtilerDriver::Cr3ReadMemory(
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
	DeviceIoControl(m_hDriver, UD_IOCTL_CR3_READ_MEMORY, &ReadMemoryInfo, sizeof(READ_MEMORY_INFO), &p, sizeof(PVOID), NULL, NULL);
	return p;
}

PVOID UtilerDriver::Cr3WriteMemory(
	PVOID pEprocess,
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
	pWriteMemoryInfo->ulWriteSize = ulWriteLength;
	RtlCopyMemory(pWriteMemoryInfo->Data, pBuffer, ulWriteLength);
	PVOID p = NULL;
	DeviceIoControl(m_hDriver, UD_IOCTL_CR3_WRITE_MEMORY, pWriteMemoryInfo, ulStructSize, &p, sizeof(PVOID), NULL, NULL);
	FreeMemory(pWriteMemoryInfo);
	return p;
}

BOOL UtilerDriver::TerminateProcess(PVOID pEprocess) {
	if (m_hDriver == INVALID_HANDLE_VALUE || !pEprocess) {
		return FALSE;
	}

	return DeviceIoControl(m_hDriver, UD_IOCTL_TERMINATE_PROCESS, &pEprocess, sizeof(PVOID), NULL, 0, NULL, NULL);
}

