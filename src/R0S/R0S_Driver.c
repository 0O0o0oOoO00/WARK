#include "R0S_Driver.h"

typedef struct _NTOSKRNL_INFO {
	PVOID pNtoskrnlBase;
	UNICODE_STRING szNtoskrnlName;
}NTOSKRNL_INFO, * PNTOSKRNL_INFO;

NTSTATUS EnumDriverLdr(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ ENUM_DRIVER_LDR_CALLBACK Callback,
	_In_ PVOID Parameter
) {
	PNOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY pLdr = pDriverObject->DriverSection;
	PLIST_ENTRY pListEntry = &(pLdr->InLoadOrderLinks);
	PLIST_ENTRY pFirstList = pListEntry;
	do {
		pLdr = CONTAINING_RECORD(pListEntry, NOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		ENUM_STATUS EnumStatus = Callback(pLdr, Parameter);
		switch (EnumStatus) {
		case ENUM_CONTINUE:
			break;
		case ENUM_BREAK:
			goto SuccessReturn;
			break;
		case ENUM_ERROR:
			return STATUS_UNSUCCESSFUL;
			break;
		default:
			return STATUS_UNSUCCESSFUL;
			break;
		}
		pListEntry = pListEntry->Flink;
	} while (pListEntry != pFirstList);
SuccessReturn:
	return STATUS_SUCCESS;
}

ENUM_STATUS GetNtoskrnlBaseCallback(
	_In_ PNOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY pLdr, 
	_In_opt_ PVOID Parameter
) {
	PNTOSKRNL_INFO pNtoskrnlInfo = (PNOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY)Parameter;
	if (RtlCompareUnicodeString(&(pNtoskrnlInfo->szNtoskrnlName), &(pLdr->BaseDllName), TRUE) == 0) {
		pNtoskrnlInfo->pNtoskrnlBase = pLdr->DllBase;
		return ENUM_BREAK;
	}
	return ENUM_CONTINUE;
}

PVOID GetNtoskrnlBase(
	_In_ PDRIVER_OBJECT pDriverObject
) {
	NTOSKRNL_INFO NtoskrnlInfo = { 0 };
	RtlInitUnicodeString(&(NtoskrnlInfo.szNtoskrnlName), NTOSKRNL_NAME);
	EnumDriverLdr(pDriverObject, GetNtoskrnlBaseCallback, &NtoskrnlInfo);
	return NtoskrnlInfo.pNtoskrnlBase;
}