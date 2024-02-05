#include "ProcessDetailWnd.hpp"
#include "DC.hpp"
#include <QDateTime>

extern Driver* g_pDriver;

ProcessDetailWnd::ProcessDetailWnd(PVOID pEprocess, QWidget* parent)
    : QWidget(parent)
    , m_pEprocess(NULL)
{
    m_Ui.setupUi(this);
    QWidget::setFixedSize(QWidget::size());

    m_Ui.EprocessDetailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_Ui.PcbDetailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_Ui.PebDetailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_Ui.EprocessDetailTable->setRowCount(EPROCESS_DETAIL_PROPERTY_COUNT);
    m_Ui.PcbDetailTable->setRowCount(PCB_DETAIL_PROPERTY_COUNT);
    m_Ui.PebDetailTable->setRowCount(PEB_DETAIL_PROPERTY_COUNT);

    connect(m_Ui.ProcessDetailRefreshButton, &QPushButton::clicked, this, &ProcessDetailWnd::RefershProcessDetail);

    for (ULONG i = 0; i < EPROCESS_DETAIL_PROPERTY_COUNT; i++) {
        m_Ui.EprocessDetailTable->setItem(i, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_NAME_LIST[i])));
    }
    for (ULONG i = 0; i < PCB_DETAIL_PROPERTY_COUNT; i++) {
        m_Ui.PcbDetailTable->setItem(i, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_NAME_LIST[i])));
    }
    for (ULONG i = 0; i < PEB_DETAIL_PROPERTY_COUNT; i++) {
        m_Ui.PebDetailTable->setItem(i, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_NAME_LIST[i])));
    }

    if (pEprocess) {
        m_pEprocess = pEprocess;
        RefershProcessDetail();
    }

}

ProcessDetailWnd::~ProcessDetailWnd() {

}

void ProcessDetailWnd::RefershProcessDetail(bool checked) {

    if (!m_pEprocess) {
        return;
    }

    ProcessDetailInfo Info = g_pDriver->CollectProcessDetailInfo(m_pEprocess);

    ULONG i = 0;
    ProcessDetailEprocess EprocessInfo = Info.m_Eprocess;
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_pEprocess));
    ADD_EPROCESS_TABLE_VALUE(Q_NUMBER_STRING(EprocessInfo.m_UniqueProcessId));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_CreateTime.QuadPart));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullPeakVirtualSize));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullVirtualSize));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullMmReserved));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ulCookie));
    ADD_EPROCESS_TABLE_VALUE(Q_NUMBER_STRING(EprocessInfo.m_ullOwnerProcessId));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_pDebugPort));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ucPriorityClass));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_pSecurityPort));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_VadHint));
    ADD_EPROCESS_TABLE_VALUE(Q_NUMBER_STRING(EprocessInfo.m_ullVadCount));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullVadPhysicalPages));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullVadPhysicalPagesLimit));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullSecurityDomain));
    ADD_EPROCESS_TABLE_VALUE(Q_HEX_NUMBER_STRING(EprocessInfo.m_ullParentSecurityDomain));

    i = 0;
    ProcessDetailPcb PcbInfo = Info.m_Pcb;
    ADD_PCB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PcbInfo.m_pPcb));
    ADD_PCB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PcbInfo.m_ullDirectoryTableBase));
    ADD_PCB_TABLE_VALUE(Q_NUMBER_STRING(PcbInfo.m_ulKernelTime));
    ADD_PCB_TABLE_VALUE(Q_NUMBER_STRING(PcbInfo.m_ulUserTime));
    ADD_PCB_TABLE_VALUE(Q_NUMBER_STRING(PcbInfo.m_ulReadyTime));
    ADD_PCB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PcbInfo.m_ullUserDirectoryTableBase));
    ADD_PCB_TABLE_VALUE(Q_NUMBER_STRING(PcbInfo.m_ullKernelWaitTime));
    ADD_PCB_TABLE_VALUE(Q_NUMBER_STRING(PcbInfo.m_ullUserWaitTime));

    i = 0;
    ProcessDetailPeb PebInfo = Info.m_Peb;
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_pPeb));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_ucInheritedAddressSpace));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_ucBeingDebugged));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_pImageBaseAddress));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_pProcessHeap));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_ulSystemReserved));
    ADD_PEB_TABLE_VALUE(Q_NUMBER_STRING(PebInfo.m_ulNumberOfProcessors));
    ADD_PEB_TABLE_VALUE(Q_NUMBER_STRING(PebInfo.m_ulNumberOfHeaps));
    ADD_PEB_TABLE_VALUE(Q_NUMBER_STRING(PebInfo.m_ulMaximumNumberOfHeaps));
    ADD_PEB_TABLE_VALUE(Q_HEX_NUMBER_STRING(PebInfo.m_ulSessionId));
}
