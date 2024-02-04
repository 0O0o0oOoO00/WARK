#include "R0S_Module.h"
#include "R0S_Undocumented.h"
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"

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
	} while (pList == pFirstList);
}