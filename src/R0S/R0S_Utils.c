#include <intrin.h>
#include "R0S_Utils.h"
#include "R0S_Undocumented.h"

NTSTATUS RtlAppendMultipleUnicodeToString(
	_In_ PUNICODE_STRING pBaseString,
	_In_ ULONG ulCount,
	...
) {
	if (!pBaseString || !ulCount) {
		return STATUS_UNSUCCESSFUL;
	}
	va_list pString = NULL;
	__va_start(&pString, ulCount);
	for (ULONG i = 0; i < ulCount; i++) {
		if (!NT_SUCCESS(RtlAppendUnicodeToString(pBaseString, _crt_va_arg(pString, PWCHAR)))) {
			return STATUS_UNSUCCESSFUL;
		}
	}
	_crt_va_end(pString);
	return STATUS_SUCCESS;
}

NTSTATUS RtlAppendMultipleUnicodeStringToString(
	_In_ PUNICODE_STRING pBaseString,
	_In_ ULONG ulCount,
	...
) {
	if (!pBaseString || !ulCount) {
		return STATUS_UNSUCCESSFUL;
	}
	va_list pString = NULL;
	__va_start(&pString, ulCount);
	for (ULONG i = 0; i < ulCount; i++) {
		if (!NT_SUCCESS(RtlAppendUnicodeStringToString(pBaseString, _crt_va_arg(pString, PUNICODE_STRING)))) {
			return STATUS_UNSUCCESSFUL;
		}
	}
	_crt_va_end(pString);
	return STATUS_SUCCESS;
}

PVOID MmAllocateZeroedMemory(
	_In_ POOL_TYPE PoolType,
	_In_ SIZE_T NumberOfBytes
) {
	if (!NumberOfBytes) {
		return NULL;
	}

	KIRQL OldIrql;
	KeRaiseIrql(PASSIVE_LEVEL, &OldIrql);
	PVOID p = ExAllocatePool(PoolType, NumberOfBytes);
	KeLowerIrql(OldIrql);
	RtlZeroMemory(p, NumberOfBytes);
	return p;
}

SIZE_T RtlMultipleUnicodeStringLengthBytes(
	_In_ ULONG ulCount,
	...
) {
	if (!ulCount) {
		return 0;
	}

	SIZE_T LengthBytes = 0;
	va_list pStringVaList = NULL;
	__va_start(&pStringVaList, ulCount);
	for (ULONG i = 0; i < ulCount; i++) {
		PUNICODE_STRING pString = _crt_va_arg(pStringVaList, PUNICODE_STRING);
		if (!pString) {
			return 0;
		}
		LengthBytes += pString->Length;
	}
	_crt_va_end(pStringVaList);
	LengthBytes += 2;
	return LengthBytes;
}

NTSTATUS RtlEmptyUnicodeString(
	_Out_ PUNICODE_STRING pString,
	_In_ SIZE_T NumberOfBytes
) {
	RtlInitEmptyUnicodeString(pString, MmAllocateZeroedNonPagedMemory(NumberOfBytes), NumberOfBytes);
	return STATUS_SUCCESS;
}

VOID RtlFreeUnicodeStringBuffer(
	_Inout_ PUNICODE_STRING pString
) {
	if (pString) {
		return;
	}
	pString->Length = 0;
	pString->MaximumLength = 0;
	if (pString->Buffer) {
		ExFreePool(pString->Buffer);
		pString->Buffer = NULL;
	}
}

PVOID MmAllocateR3Memory(
	_In_ PEPROCESS pEprocess,
	_In_ SIZE_T Size
) {
	if (!pEprocess || !Size) {
		return NULL;
	}

	PVOID R3Address = NULL;
	HANDLE hProcess = NULL;
	
	if (!NT_SUCCESS(ObOpenObjectByPointer(pEprocess, OBJ_KERNEL_HANDLE, NULL, 0, *PsProcessType, KernelMode, &hProcess))) {
		return NULL;
	}
	if (!NT_SUCCESS(ZwAllocateVirtualMemory(hProcess, &R3Address, NULL, &Size, MEM_COMMIT, PAGE_READWRITE))) {
		return NULL;
	}

	return R3Address;
}

PVOID SendDataToR3(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulLength
) {
	if (!pEprocess || !pBuffer || !ulLength) {
		return NULL;
	}

	PVOID R3Address = MmAllocateR3Memory(pEprocess, ulLength);
	if (!R3Address) {
		return NULL;
	}

	SIZE_T ReturnSize = 0;
	if (!NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), pBuffer, pEprocess, R3Address, ulLength, KernelMode, &ReturnSize))) {
		return NULL;
	}
	return R3Address;
}

NTSTATUS SwitchCr3(
	_In_ ULONGLONG ullNewCr3,
	_Out_opt_ PULONGLONG pOldCr3
) {
	if (!ullNewCr3) {
		return STATUS_UNSUCCESSFUL;
	}

	_disable();
	ULONGLONG ullOldCr3 = __readcr3();
	__writecr3(ullNewCr3);
	_enable();

	if (pOldCr3) {
		*pOldCr3 = ullOldCr3;
	}

	return STATUS_SUCCESS;;
}