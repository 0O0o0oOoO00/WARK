#pragma once

#include <QtWidgets/QWidget>
#include "ProcessDetailWndUI.h"
#include <Windows.h>
#include "Util.hpp"

static CONST CHAR* EPROCESS_DETAIL_PROPERTY_NAME_LIST[] = {
    "Eprocess",
    "UniqueProcessId",
    "CreateTime",
    "PeakVirtualSize",
    "VirtualSize",
    "MmReserved",
    "Cookie",
    "OwnerProcessId",
    "DebugPort",
    "PriorityClass",
    "SecurityPort",
    "VadHint",
    "VadCount",
    "VadPhysicalPages",
    "VadPhysicalPagesLimit",
    "SecurityDomain",
    "ParentSecurityDomain"
};

#define EPROCESS_DETAIL_PROPERTY_INDEX 0
#define EPROCESS_DETAIL_VALUE_INDEX 1

#define EPROCESS_DETAIL_PROPERTY_COUNT \
    (sizeof(EPROCESS_DETAIL_PROPERTY_NAME_LIST) / sizeof(CHAR*))

#define ADD_EPROCESS_TABLE_VALUE(text) \
    ADD_TABLE_ITEM(EprocessDetailTable, EPROCESS_DETAIL_VALUE_INDEX, text)

static CONST CHAR* PCB_DETAIL_PROPERTY_NAME_LIST[] = {
    "Pcb",
    "DirectoryTableBase",
    "KernelTime",
    "UserTime",
    "ReadyTime",
    "UserDirectoryTableBase",
    "KernelWaitTime",
    "UserWaitTime"
};

#define PCB_DETAIL_PROPERTY_INDEX 0
#define PCB_DETAIL_VALUE_INDEX 1

#define PCB_DETAIL_PROPERTY_COUNT \
    (sizeof(PCB_DETAIL_PROPERTY_NAME_LIST) / sizeof(CHAR*))

#define ADD_PCB_TABLE_VALUE(text) \
    ADD_TABLE_ITEM(PcbDetailTable, PCB_DETAIL_VALUE_INDEX, text)

static CONST CHAR* PEB_DETAIL_PROPERTY_NAME_LIST[] = {
    "Peb",
    "InheritedAddressSpace",
    "BeingDebugged",
    "ImageBaseAddress",
    "ProcessHeap",
    "SystemReserved",
    "NumberOfProcessors",
    "NumberOfHeaps",
    "MaximumNumberOfHeaps",
    "SessionId"
};

#define PEB_DETAIL_PROPERTY_INDEX 0
#define PEB_DETAIL_VALUE_INDEX 1

#define PEB_DETAIL_PROPERTY_COUNT \
    (sizeof(PEB_DETAIL_PROPERTY_NAME_LIST) / sizeof(CHAR*))

#define ADD_PEB_TABLE_VALUE(text) \
    ADD_TABLE_ITEM(PebDetailTable, PEB_DETAIL_VALUE_INDEX, text)

class ProcessDetailWnd : public QWidget {
    Q_OBJECT

private:
    Ui::ProcessDetailWndClass m_Ui;

    PVOID m_pEprocess;

public:
    ProcessDetailWnd(PVOID pEprocess, QWidget* parent = nullptr);
    ~ProcessDetailWnd();

private:
    void RefershProcessDetail(bool checked = false);
};