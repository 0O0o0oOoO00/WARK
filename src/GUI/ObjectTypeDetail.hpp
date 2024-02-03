#pragma once

#include <QtWidgets/QWidget>
#include "ObjectTypeDetailUI.h"
#include <Windows.h>

class ObjectTypeDetail : public QWidget {
    Q_OBJECT

private:
    Ui::ObjectTypeDetailClass m_Ui;

public:
    ObjectTypeDetail(QWidget* parent = nullptr);
    ~ObjectTypeDetail();

private:
    
};