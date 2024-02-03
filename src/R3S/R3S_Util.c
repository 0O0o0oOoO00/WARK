#include "R3S_Util.h"

 
PVOID AllocateZeroedMemory(
	_In_ SIZE_T dwSize
) {
	if (!dwSize) {
		return NULL;
	}
	PVOID p = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!p) {
		return NULL;
	}
	RtlZeroMemory(p, dwSize);
	return p;
}

VOID FreeMemory(
	_In_ PVOID p
) {
	if (p) {
		VirtualFree(p, 0, MEM_RELEASE);
	}
}