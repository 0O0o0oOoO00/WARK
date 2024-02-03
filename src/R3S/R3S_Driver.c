#include "R3S_Driver.h"

BOOL LoadDriver(
	_In_ LPCWSTR pServiceName,
	_In_ LPCWSTR pDriverFullPath
) {
	if (!pServiceName || !pDriverFullPath) {
		return FALSE;
	}

	SC_HANDLE hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hManager) {
		return FALSE;
	}

	SC_HANDLE hService = CreateService(
		hManager,
		pServiceName,
		pServiceName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		pDriverFullPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	if (hService) {
		StartService(hService, NULL, NULL);
		goto CloseHandles;
	}

	if (GetLastError() != ERROR_SERVICE_EXISTS) {
		CloseServiceHandle(hManager);
		return FALSE;
	}
	hService = OpenService(hManager, pServiceName, SERVICE_ALL_ACCESS);
	if (!hService) {
		CloseServiceHandle(hManager);
		return FALSE;
	}
	/*
	SERVICE_STATUS ServiceStatus = { 0 };
	if (!ControlService(hService, SERVICE_CONTROL_CONTINUE, &ServiceStatus)) {
		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);
		return FALSE;
	}
	*/
	StartService(hService, NULL, NULL);

CloseHandles:
	CloseServiceHandle(hService);
	CloseServiceHandle(hManager);

	return TRUE;
}

BOOL UnloadDriver(
	_In_ LPCWSTR pServiceName
) {
	if (!pServiceName) {
		return FALSE;
	}

	SC_HANDLE hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hManager) {
		return FALSE;
	}

	SC_HANDLE hService = OpenService(hManager, pServiceName, SERVICE_ALL_ACCESS);
	if (!hService) {
		CloseServiceHandle(hManager);
		return FALSE;
	}

	SERVICE_STATUS ServiceStatus = { 0 };
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus)) {
		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hManager);

	return TRUE;
}