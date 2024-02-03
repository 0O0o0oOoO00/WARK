#include "R0S_Process.h"

NTSTATUS EnumEprocess(
	_In_ ENUM_EPROCESS_CALLBACK Callback,
	_In_ PVOID Parameter
) {
	if (!Callback) {
		return STATUS_UNSUCCESSFUL;
	}

	PNOK_INCOMPLETE_EPROCESS pEprocess = (PNOK_INCOMPLETE_EPROCESS)PsInitialSystemProcess;
	PLIST_ENTRY pListEntry = &(pEprocess->ActiveProcessLinks);
	PLIST_ENTRY pFirstListEntry = pListEntry;

	do {
		pEprocess = CONTAINING_RECORD(pListEntry, NOK_INCOMPLETE_EPROCESS, ActiveProcessLinks);
		ENUM_STATUS EnumStatus = Callback(pEprocess, Parameter);
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
	} while (pListEntry != pFirstListEntry);
SuccessReturn:
	return STATUS_SUCCESS;
}