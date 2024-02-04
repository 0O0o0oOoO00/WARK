#include "Collector.hpp"
#include "PS_CD_DriverDefine.h"
#include "PS_CD_ControlCode.h"
#include "R3S_Driver.h"

CollectorDriver::CollectorDriver()
	: m_hDriver(INVALID_HANDLE_VALUE)
{
	WCHAR szCurrentDirectory[MAX_PATH] = { 0 };
	if (!GetCurrentDirectory(MAX_PATH, szCurrentDirectory)) {
		return;
	}
	if (!LoadDriver(CD_DRIVER_SERVICE_NAME, (WSTRING(szCurrentDirectory) + L"\\" + WSTRING(CD_DRIVER_NAME)).c_str())) {
		return;
	}

	m_hDriver = CreateFile(CD_DEVICE_SYMBOL_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return;
	}
	
	ULONG ulCurrentProcessId = GetCurrentProcessId();
	if (!DeviceIoControl(m_hDriver, CD_IOCTL_SET_TARGET_PROCESS, &ulCurrentProcessId, sizeof(ULONG), NULL, 0, NULL, NULL)) {
		CloseHandle(m_hDriver);
		m_hDriver = INVALID_HANDLE_VALUE;
	}
}

CollectorDriver::~CollectorDriver() {
	if (m_hDriver != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hDriver);
	}
}

BOOL CollectorDriver::Unload() {
	if (m_hDriver == INVALID_HANDLE_VALUE) {
		return TRUE;
	}
	BOOL Retval = UnloadDriver(CD_DRIVER_SERVICE_NAME);
	if (Retval) {
		m_hDriver = INVALID_HANDLE_VALUE;
	}
	return Retval;
}

Vector<ProcessInfo> CollectorDriver::CollectProcessInfo() {
	Vector<ProcessInfo> ProcessInfoVector = *new Vector<ProcessInfo>;
	PPROCESS_INFO* pProcessInfoList = NULL;
	DWORD dwCount = 0;

	if (m_hDriver == INVALID_HANDLE_VALUE) {
		goto Return;
	}

	if (!DeviceIoControl(m_hDriver, CD_IOCTL_COLLECT_PROCESS_INFO, NULL, 0, &pProcessInfoList, sizeof(PPROCESS_INFO*), &dwCount, NULL)) {
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

Vector<ObjectTypeInfo> CollectorDriver::CollectObjectTypeInfo() {
	Vector<ObjectTypeInfo> ObjectInfoVector = *new Vector<ObjectTypeInfo>;
	POBJECT_TYPE_INFO* pObjectInfoList = NULL;
	DWORD dwCount = 0;

	if (m_hDriver == INVALID_HANDLE_VALUE) {
		goto Return;
	}

	if (!DeviceIoControl(m_hDriver, CD_IOCTL_COLLECT_OBJECT_TYPE_INFO, NULL, 0, &pObjectInfoList, sizeof(POBJECT_TYPE_INFO*), &dwCount, NULL)) {
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

ProcessDetailInfo CollectorDriver::CollectProcessDetailInfo(PVOID pEprocess) {
	PPROCESS_DETAIL pProcessDetail = NULL;

	if (m_hDriver == INVALID_HANDLE_VALUE || !pEprocess) {
		goto Error;
	}

	if (!DeviceIoControl(m_hDriver, CD_IOCTL_COLLECT_PROCESS_DETAIL, &pEprocess, sizeof(PVOID), &pProcessDetail, sizeof(PPROCESS_DETAIL), NULL, NULL)) {
		goto Error;
	}
	if (!pProcessDetail) {
		goto Error;
	}

	return ProcessDetailInfo(pProcessDetail);
Error:
	return ProcessDetailInfo();
}

ObjectTypeDetail CollectorDriver::CollectObjectTypeDetail(PVOID pObjectType) {
	POBJECT_TYPE_DETAIL pDetail = NULL;

	if (m_hDriver == INVALID_HANDLE_VALUE || !pObjectType) {
		goto Error;
	}

	if (!DeviceIoControl(m_hDriver, CD_IOCTL_COLLECT_OBJECT_TYPE_DETAIL, &pObjectType, sizeof(PVOID), &pDetail, sizeof(POBJECT_TYPE_DETAIL), NULL, NULL)) {
		goto Error;
	}
	if (!pDetail) {
		goto Error;
	}

	return ObjectTypeDetail(pDetail);
Error:
	return ObjectTypeDetail();
}