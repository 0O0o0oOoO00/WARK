#pragma once
#include <ntifs.h>

// due to 8-byte alignment, the size of this struct is 0x2c8, not 0x2c4
typedef struct _NOK_INCOMPLETE_PEB {
    UCHAR InheritedAddressSpace;                                            //0x0
    CHAR o1[0x1];
    UCHAR BeingDebugged;                                                    //0x2
    CHAR o2[0xd];
    PVOID ImageBaseAddress;                                                 //0x10
    CHAR o3[0x18];
    PVOID ProcessHeap;                                                      //0x30
    CHAR o4[0x28];
    ULONG SystemReserved;                                                   //0x60
    CHAR o5[0x54];
    ULONG NumberOfProcessors;                                               //0xb8
    CHAR o6[0x2c];
    ULONG NumberOfHeaps;                                                    //0xe8
    ULONG MaximumNumberOfHeaps;                                             //0xec
    CHAR o7[0x1d0];
    ULONG SessionId;                                                        //0x2c0
}NOK_INCOMPLETE_PEB, * PNOK_INCOMPLETE_PEB;

typedef struct _NOK_INCOMPLETE_KPROCESS {
    CHAR o1[0x28];
    ULONGLONG DirectoryTableBase;                                           //0x28
    CHAR o2[0x34c];
    ULONG KernelTime;                                                       //0x37c
    ULONG UserTime;                                                         //0x380
    ULONG ReadyTime;                                                        //0x384
    ULONGLONG UserDirectoryTableBase;                                       //0x388
    CHAR o3[0x58];
    ULONGLONG KernelWaitTime;                                               //0x3e8
    ULONGLONG UserWaitTime;                                                 //0x3f0
}NOK_INCOMPLETE_KPROCESS, * PNOK_INCOMPLETE_KPROCESS;

typedef struct _NOK_INCOMPLETE_EPROCESS {
    NOK_INCOMPLETE_KPROCESS Pcb;                                            //0x0
    CHAR o1[0x48];
    PVOID UniqueProcessId;                                                  //0x440
    LIST_ENTRY ActiveProcessLinks;                                          //0x448
    CHAR o2[0x10];
    LARGE_INTEGER CreateTime;                                               //0x468
    CHAR o3[0x20];
    ULONGLONG PeakVirtualSize;                                              //0x490
    ULONGLONG VirtualSize;                                                  //0x498
    CHAR o4[0x20];
    ULONGLONG MmReserved;                                                   //0x4c0
    CHAR o5[0x60];
    ULONG Cookie;                                                           //0x528
    CHAR o6[0x1c];
    ULONGLONG OwnerProcessId;                                               //0x548
    PNOK_INCOMPLETE_PEB Peb;                                                //0x550
    CHAR o7[0x20];
    PVOID DebugPort;                                                        //0x578
    CHAR o8[0x20];
    PFILE_OBJECT ImageFilePointer;                                          //0x5a0
    CHAR o9[0xf];
    UCHAR PriorityClass;                                                    //0x5b7
    PVOID SecurityPort;                                                     //0x5b8
    CHAR o10[0x220];
    PVOID VadHint;                                                          //0x7e0
    ULONGLONG VadCount;                                                     //0x7e8
    ULONGLONG VadPhysicalPages;                                             //0x7f0
    ULONGLONG VadPhysicalPagesLimit;                                        //0x7f8
    CHAR o11[0x1e0];
    ULONGLONG SecurityDomain;                                               //0x9e0
    ULONGLONG ParentSecurityDomain;                                         //0x9e8
}NOK_INCOMPLETE_EPROCESS, * PNOK_INCOMPLETE_EPROCESS;

typedef struct _NOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;                                            //0x0
    LIST_ENTRY InMemoryOrderLinks;                                          //0x10
    LIST_ENTRY InInitializationOrderLinks;                                  //0x20
    PVOID DllBase;                                                          //0x30
    PVOID EntryPoint;                                                       //0x38
    ULONGLONG SizeOfImage;                                                  //0x40
    UNICODE_STRING FullDllName;                                             //0x48
    UNICODE_STRING BaseDllName;                                             //0x58
}NOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY, * PNOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY;

//0x78 bytes (sizeof)
typedef struct _NOK_INCOMPLETE_OBJECT_TYPE_INITIALIZER {
    USHORT Length;                                                          //0x0
    union {
        USHORT ObjectTypeFlags;                                             //0x2
        struct {
            UCHAR CaseInsensitive : 1;                                      //0x2
            UCHAR UnnamedObjectsOnly : 1;                                   //0x2
            UCHAR UseDefaultObject : 1;                                     //0x2
            UCHAR SecurityRequired : 1;                                     //0x2
            UCHAR MaintainHandleCount : 1;                                  //0x2
            UCHAR MaintainTypeList : 1;                                     //0x2
            UCHAR SupportsObjectCallbacks : 1;                              //0x2
            UCHAR CacheAligned : 1;                                         //0x2
            UCHAR UseExtendedParameters : 1;                                //0x3
            UCHAR Reserved : 7;                                             //0x3
        };
    };
    ULONG ObjectTypeCode;                                                   //0x4
    ULONG InvalidAttributes;                                                //0x8
    GENERIC_MAPPING GenericMapping;                                         //0xc
    ULONG ValidAccessMask;                                                  //0x1c
    ULONG RetainAccess;                                                     //0x20
    POOL_TYPE PoolType;                                                     //0x24
    ULONG DefaultPagedPoolCharge;                                           //0x28
    ULONG DefaultNonPagedPoolCharge;                                        //0x2c
    PVOID DumpProcedure;                                                    //0x30
    PVOID OpenProcedure;                                                    //0x38
    PVOID CloseProcedure;                                                   //0x40
    PVOID DeleteProcedure;                                                  //0x48
    union {
        PVOID ParseProcedure;                                               //0x50
        PVOID ParseProcedureEx;                                             //0x50
    };
    PVOID SecurityProcedure;                                                //0x58
    PVOID QueryNameProcedure;                                               //0x60
    PVOID OkayToCloseProcedure;                                             //0x68
    ULONG WaitObjectFlagMask;                                               //0x70
    USHORT WaitObjectFlagOffset;                                            //0x74
    USHORT WaitObjectPointerOffset;                                         //0x76
}NOK_INCOMPLETE_OBJECT_TYPE_INITIALIZER, * PNOK_INCOMPLETE_OBJECT_TYPE_INITIALIZER;

typedef struct _NOK_INCOMPLETE_OBJECT_TYPE {
    LIST_ENTRY TypeList;                                                    //0x0
    UNICODE_STRING Name;                                                    //0x10
    PVOID DefaultObject;                                                    //0x20
    UCHAR Index;                                                            //0x28
    ULONG TotalNumberOfObjects;                                             //0x2c
    ULONG TotalNumberOfHandles;                                             //0x30
    ULONG HighWaterNumberOfObjects;                                         //0x34
    ULONG HighWaterNumberOfHandles;                                         //0x38
    NOK_INCOMPLETE_OBJECT_TYPE_INITIALIZER TypeInfo;                        //0x40
    CHAR o1[0x8];
    ULONG Key;                                                              //0xc0
    LIST_ENTRY CallbackList;                                                //0xc8
}NOK_INCOMPLETE_OBJECT_TYPE, * PNOK_INCOMPLETE_OBJECT_TYPE;

//0x20 bytes (sizeof)
typedef struct _NON_PAGED_DEBUG_INFO {
    USHORT Signature;                                                       //0x0
    USHORT Flags;                                                           //0x2
    ULONG Size;                                                             //0x4
    USHORT Machine;                                                         //0x8
    USHORT Characteristics;                                                 //0xa
    ULONG TimeDateStamp;                                                    //0xc
    ULONG CheckSum;                                                         //0x10
    ULONG SizeOfImage;                                                      //0x14
    ULONGLONG ImageBase;                                                    //0x18
}NON_PAGED_DEBUG_INFO, * PNON_PAGED_DEBUG_INFO;

//0xa0 bytes (sizeof)
typedef struct _KLDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;                                            //0x0
    PVOID ExceptionTable;                                                   //0x10
    ULONG ExceptionTableSize;                                               //0x18
    PVOID GpValue;                                                          //0x20
    NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;                                //0x28
    PVOID DllBase;                                                          //0x30
    PVOID EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    UNICODE_STRING FullDllName;                                             //0x48
    UNICODE_STRING BaseDllName;                                             //0x58
    ULONG Flags;                                                            //0x68
    USHORT LoadCount;                                                       //0x6c
    union {
        USHORT SignatureLevel : 4;                                          //0x6e
        USHORT SignatureType : 3;                                           //0x6e
        USHORT Unused : 9;                                                  //0x6e
        USHORT EntireField;                                                 //0x6e
    } u1;                                                                   //0x6e
    PVOID SectionPointer;                                                   //0x70
    ULONG CheckSum;                                                         //0x78
    ULONG CoverageSectionSize;                                              //0x7c
    PVOID CoverageSection;                                                  //0x80
    PVOID LoadedImports;                                                    //0x88
    PVOID Spare;                                                            //0x90
    ULONG SizeOfImageNotRounded;                                            //0x98
    ULONG TimeDateStamp;                                                    //0x9c
}KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;