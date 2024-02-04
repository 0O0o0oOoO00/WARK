#include "R0S_Module.h"
#include "R0S_Undocumented.h"

NTSTATUS EnumModule(
	_In_ ENUM_MODULE fnCallback,
	_In_opt_ PVOID Parameter
) {
	if (!fnCallback) {
		return STATUS_UNSUCCESSFUL;
	}

	PKLDR_DATA_TABLE_ENTRY pKldr = CONTAINING_RECORD(&PsLoadedModuleList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	PLIST_ENTRY pFirstList = &PsLoadedModuleList;
	PLIST_ENTRY pList = pFirstList;

	do {
		pKldr = CONTAINING_RECORD(pList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		ENUM_STATUS EnumStatus = fnCallback(pKldr, Parameter);
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
		pList = pList->Flink;
	} while (pList != pFirstList);
SuccessReturn:
	return STATUS_SUCCESS;
}

PVOID GetNtoskrnlBase() {
	UNICODE_STRING szNtoskrnlName = { 0 };
	RtlInitUnicodeString(&szNtoskrnlName, NTOSKRNL_NAME);

	PKLDR_DATA_TABLE_ENTRY pKldr = CONTAINING_RECORD(&PsLoadedModuleList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	PLIST_ENTRY pFirstList = &PsLoadedModuleList;
	PLIST_ENTRY pList = pFirstList;
	do {
		pKldr = CONTAINING_RECORD(pList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (RtlCompareUnicodeString(&szNtoskrnlName, &(pKldr->BaseDllName), TRUE) == 0) {
			return pKldr->DllBase;
		}
		pList = pList->Flink;
	} while (pList != pFirstList);
}