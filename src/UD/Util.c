#include "Util.h"
#include "R0S_NtoskrnlStruct.h"

ULONGLONG GetProcessCr3(
	_In_ PEPROCESS pEprocess
) {
	if (!pEprocess || !MmIsAddressValid(pEprocess)) {
		return 0;
	}
	return ((PNOK_INCOMPLETE_EPROCESS)(pEprocess))->Pcb.DirectoryTableBase;
}