#include "R0S_Ioctl.h"

NTSTATUS InitializeIrpData(
	_In_ PIRP pIrp,
	_Out_ PIRP_DATA pIrpData
) {
	if (!pIrp || !pIrpData) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory(pIrpData, sizeof(IRP_DATA));
	pIrpData->pIrp = pIrp;
	pIrpData->pIoStack = IoGetCurrentIrpStackLocation(pIrp);
	return STATUS_SUCCESS;
}