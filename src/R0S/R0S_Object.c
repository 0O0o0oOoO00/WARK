#include "R0S_Object.h"
#include "R0S_Undocumented.h"
#include "R0S_NtoskrnlOffset.h"
#include "PS_Util.h"


NTSTATUS EnumObject(
	_In_ PVOID pNtoskrnlBase,
	_In_ ENUM_OBJECT_CALLBACK Callback,
	_In_ PVOID Parameter
) {
	if (!pNtoskrnlBase || !Callback) {
		return STATUS_UNSUCCESSFUL;
	}

	POBJECT_TYPE* pObTypeIndexTable = CALC_ADDRESS(pNtoskrnlBase, OFFSET_ObTypeIndexTable);

	ULONG i = 2;
	do {
		ENUM_STATUS EnumStatus = Callback(pObTypeIndexTable[i], Parameter);
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
		i++;
	} while (pObTypeIndexTable[i] != NULL);
	
SuccessReturn:
	return STATUS_SUCCESS;
}