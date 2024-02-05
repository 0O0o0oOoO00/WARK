#pragma once
#include <QtWidgets/QWidget>
#include <QString.h>

#define ADD_TABLE_ITEM(table, index, text) \
    m_Ui.table->setItem(i++, index, new QTableWidgetItem(QString(text)))

#define Q_HEX_NUMBER_STRING(n) \
    ("0x" + QString::number((ULONGLONG)(n), 16))

#define Q_NUMBER_STRING(n) \
    QString::number((ULONGLONG)(n))