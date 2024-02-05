#include "Ioctl.h"
#include "Util.h"

NTSTATUS InitializeIoPackage(
	_Out_ PIO_PACKAGE pIoPackage,
	_In_ PIRP_DATA pIrpData
) {
	if (!pIoPackage || !pIrpData) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory((PVOID)pIoPackage, sizeof(IO_PACKAGE));

	ULONG IoctlCode = pIrpData->pIoStack->Parameters.DeviceIoControl.IoControlCode;
	ULONG IoctlMethod = METHOD_FROM_CTL_CODE(IoctlCode);

	pIoPackage->IoMethod = IoctlMethod;
	pIoPackage->ulInputBufferLength = pIrpData->pIoStack->Parameters.DeviceIoControl.InputBufferLength;
	pIoPackage->ulOutputBufferLength = pIrpData->pIoStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch (IoctlMethod) {
	case METHOD_BUFFERED:
		pIoPackage->pInputBuffer = pIrpData->pIrp->AssociatedIrp.SystemBuffer;
		pIoPackage->pOutputBuffer = pIrpData->pIrp->AssociatedIrp.SystemBuffer;
		break;
	case METHOD_OUT_DIRECT:
		pIoPackage->pOutputBuffer = MmGetSystemAddressForMdlSafe(pIrpData->pIrp->MdlAddress, NormalPagePriority);
	case METHOD_IN_DIRECT:
		pIoPackage->pInputBuffer = pIrpData->pIrp->AssociatedIrp.SystemBuffer;
		break;
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
}

NTSTATUS WriteToIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_In_ PVOID pBuffer,
	_In_ SIZE_T LengthToWrite,
	_Out_ SIZE_T* pWrittenLength,
	_In_ BOOLEAN bForce
) {
	if (!pIoPackage || !pBuffer || !LengthToWrite) {
		return STATUS_UNSUCCESSFUL;
	}
	if (!pIoPackage->pOutputBuffer) {
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	SIZE_T WriteLength = 0;
	SIZE_T OutputBufferRemainingLength = pIoPackage->ulOutputBufferLength - pIoPackage->ulOuttputBufferOperatedLength;

	if (LengthToWrite <= OutputBufferRemainingLength) {
		WriteLength = LengthToWrite;
		Status = STATUS_SUCCESS;
	}
	else {
		if (bForce) {
			WriteLength = OutputBufferRemainingLength;
			Status = STATUS_SUCCESS;
		}
	}

	RtlCopyMemory(
		CALC_ADDRESS(pIoPackage->pOutputBuffer, pIoPackage->ulOuttputBufferOperatedLength),
		pBuffer,
		WriteLength
	);

	pIoPackage->ulOuttputBufferOperatedLength += WriteLength;

	if (pWrittenLength) {
		*pWrittenLength = WriteLength;
	}

	return Status;
}

NTSTATUS ReadIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_Out_ PVOID pBuffer,
	_In_ SIZE_T LengthToRead,
	_Out_ SIZE_T* pReadLength,
	_In_ BOOLEAN bForce
) {
	if (!pIoPackage || !pBuffer || !LengthToRead) {
		return STATUS_UNSUCCESSFUL;
	}
	if (!pIoPackage->pInputBuffer) {
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	SIZE_T ReadLength = 0;
	SIZE_T InputBufferRemainingLength = pIoPackage->ulInputBufferLength - pIoPackage->ulInputBufferOperatedLength;

	if (LengthToRead <= InputBufferRemainingLength) {
		ReadLength = LengthToRead;
		Status = STATUS_SUCCESS;
	}
	else {
		if (bForce) {
			ReadLength = InputBufferRemainingLength;
			Status = STATUS_SUCCESS;
		}
	}

	RtlCopyMemory(
		pBuffer,
		CALC_ADDRESS(pIoPackage->pInputBuffer, pIoPackage->ulInputBufferOperatedLength),
		ReadLength
	);

	pIoPackage->ulInputBufferOperatedLength += ReadLength;

	if (pReadLength) {
		*pReadLength = ReadLength;
	}

	return Status;
}

ULONG OutputBufferLengthOfIoPackage(
	_In_ PIO_PACKAGE pIoPackage
) {
	return pIoPackage->ulOutputBufferLength;
}

ULONG InputBufferLengthOfIoPackage(
	_In_ PIO_PACKAGE pIoPackage
) {
	return pIoPackage->ulInputBufferLength;
}

NTSTATUS ReadEntireIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_Out_ PVOID pBuffer,
	_Out_opt_ SIZE_T* pReadLength
) {
	return ReadIoPackage(pIoPackage, pBuffer, pIoPackage->ulInputBufferLength, pReadLength, FALSE);
}

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