#include "ObjectTypeDetail.hpp"

ObjectTypeDetail::ObjectTypeDetail(QWidget* parent)
    : QWidget(parent)
{
    m_Ui.setupUi(this);
    QWidget::setFixedSize(QWidget::size());
}

ObjectTypeDetail::~ObjectTypeDetail() {
}