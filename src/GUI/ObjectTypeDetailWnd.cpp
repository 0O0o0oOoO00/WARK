#include "ObjectTypeDetailWnd.hpp"
#include "DC.hpp"

extern Driver* g_pDriver;

ObjectTypeDetailWnd::ObjectTypeDetailWnd(
    PVOID pObjectType,
    QWidget* parent
): QWidget(parent)
{
    m_pObjectType = pObjectType;
    m_Ui.setupUi(this);
    QWidget::setFixedSize(QWidget::size());

    m_Ui.InfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_Ui.InfoTable->setRowCount(OBJECT_TYPE_DETAIL_PROPERTY_COUNT);

    connect(m_Ui.ObjectTypeDetailRefreshButton, &QPushButton::clicked, this, &ObjectTypeDetailWnd::RefershObjectTypeDetail);

    for (ULONG i = 0; i < OBJECT_TYPE_DETAIL_PROPERTY_COUNT; i++) {
        m_Ui.InfoTable->setItem(i, OBJECT_TYPE_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(OBJECT_TYPE_DETAIL_PROPERTY_NAME_LIST[i])));
    }

}

ObjectTypeDetailWnd::~ObjectTypeDetailWnd() {}

void ObjectTypeDetailWnd::RefershObjectTypeDetail(bool checked){
    ObjectTypeDetail Detail = g_pDriver->CollectObjectTypeDetail(m_pObjectType);

    ULONG i = 0;
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pObjectType));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_usObjectTypeFlags));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulObjectTypeCode));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulInvalidAttributes));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulValidAccessMask));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulRetainAccess));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulPoolType));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulDefaultPagedPoolCharge));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulDefaultNonPagedPoolCharge));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pDumpProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pOpenProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pCloseProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pDeleteProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pParseProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pParseProcedureEx));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pSecurityProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pQueryNameProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_pOkayToCloseProcedure));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_ulWaitObjectFlagMask));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_usWaitObjectFlagOffset));
    ADD_INFO_TABLE_VALUE(Q_HEX_NUMBER_STRING(Detail.m_usWaitObjectPointerOffset));
}