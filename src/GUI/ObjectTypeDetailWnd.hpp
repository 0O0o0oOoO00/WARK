#pragma once

#include <QtWidgets/QWidget>
#include "ObjectTypeDetailWndUI.h"
#include <Windows.h>

static CONST CHAR* OBJECT_TYPE_DETAIL_PROPERTY_NAME_LIST[] = {
    "ObjectType",
    "ObjectTypeFlags",
    "ObjectTypeCode",
    "InvalidAttributes",
    "ValidAccessMask",
    "RetainAccess",
    "PoolType",
    "DefaultPagedPoolCharge",
    "DefaultNonPagedPoolCharge",
    "DumpProcedure",
    "OpenProcedure",
    "CloseProcedure",
    "DeleteProcedure",
    "ParseProcedure",
    "ParseProcedureEx",
    "SecurityProcedure",
    "QueryNameProcedure",
    "OkayToCloseProcedure",
    "WaitObjectFlagMask",
    "WaitObjectFlagOffset",
    "WaitObjectPointerOffset"
};

#define OBJECT_TYPE_DETAIL_PROPERTY_COUNT \
    (sizeof(OBJECT_TYPE_DETAIL_PROPERTY_NAME_LIST) / sizeof(CHAR*))

#define OBJECT_TYPE_DETAIL_PROPERTY_INDEX 0
#define OBJECT_TYPE_DETAIL_VALUE_INDEX 1

#define ADD_TABLE_ITEM(table, index, text) \
    m_Ui.table->setItem(i++, index, new QTableWidgetItem(QString(text)))

#define ADD_INFO_TABLE_VALUE(text) \
    ADD_TABLE_ITEM(InfoTable, OBJECT_TYPE_DETAIL_VALUE_INDEX, text)

#define Q_HEX_NUMBER_STRING(n) \
    ("0x" + QString::number((ULONGLONG)(n), 16))

#define Q_NUMBER_STRING(n) \
    QString::number((n))

class ObjectTypeDetailWnd : public QWidget {
    Q_OBJECT

private:
    Ui::ObjectTypeDetailWndClass m_Ui;

    PVOID m_pObjectType;

public:
    ObjectTypeDetailWnd(PVOID pObjectType, QWidget* parent = nullptr);
    ~ObjectTypeDetailWnd();

private:
    void RefershObjectTypeDetail(bool checked = false);
};