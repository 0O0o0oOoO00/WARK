#include "Driver.h"

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