#include "ProcessDetailWnd.hpp"
#include "Collector.hpp"
#include <QDateTime>

extern CollectorDriver* g_pCollectorDriver;

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

    ULONG i = 0;
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_PEPROCESS_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_UNIQUE_PROCESS_ID_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_CREATE_TIME_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_PEAK_VIRTUAL_SIZE_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_VIRTUAL_SIZE_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_MM_RESERVED_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_COOKIE_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_OWNER_PROCESS_ID_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_DEBUG_PORT_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_PRIORITY_CLASS_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_SECURITY_PORT_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_VAD_HINT_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_VAD_COUNT_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_VAD_PHYSICAL_PAGES_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_VAD_PHYSICAL_PAGES_LIMIT_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_SECURITY_DOMAIN_NAME)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(EPROCESS_DETAIL_PROPERTY_PARENT_SECURITY_DOMAIN_NAME)));

    i = 0;
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_PPCB_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_DIRECTORY_TABLE_BASE_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_KERNEL_TIME_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_USER_TIME_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_READY_TIME_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_USER_DIRECTORY_TABLE_BASE_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_KERNEL_WAIT_TIME_NAME)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PCB_DETAIL_PROPERTY_USER_WAIT_TIME_NAME)));

    i = 0;
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_PPEB_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_INHERITED_ADDRESS_SPACE_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_BEING_DEBUGGED_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_IMAGE_BASE_ADDRESS_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_PROCESS_HEAP_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_SYSTEM_RESERVED_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_NUMBER_OF_PROCESSORS_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_NUMBER_OF_HEAPS_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_MAXIMUM_NUMBER_OF_HEAPS_NAME)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_PROPERTY_INDEX, new QTableWidgetItem(QString(PEB_DETAIL_PROPERTY_SESSION_ID_NAME)));


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

    ProcessDetailInfo Info = g_pCollectorDriver->CollectProcessDetailInfo(m_pEprocess);

    ULONG i = 0;
    ProcessDetailEprocess EprocessInfo = Info.m_Eprocess;
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)EprocessInfo.m_pEprocess, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number((ULONGLONG)EprocessInfo.m_UniqueProcessId)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_CreateTime.QuadPart, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullPeakVirtualSize, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullVirtualSize, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullMmReserved, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ulCookie, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(EprocessInfo.m_ullOwnerProcessId)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)EprocessInfo.m_pDebugPort, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ucPriorityClass, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)EprocessInfo.m_pSecurityPort, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)EprocessInfo.m_VadHint, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(EprocessInfo.m_ullVadCount)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullVadPhysicalPages, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullVadPhysicalPagesLimit, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullSecurityDomain, 16)));
    m_Ui.EprocessDetailTable->setItem(i++, EPROCESS_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(EprocessInfo.m_ullParentSecurityDomain, 16)));

    i = 0;
    ProcessDetailPcb PcbInfo = Info.m_Pcb;
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PcbInfo.m_pPcb, 16)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(PcbInfo.m_ullDirectoryTableBase, 16)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PcbInfo.m_ulKernelTime)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PcbInfo.m_ulUserTime)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PcbInfo.m_ulReadyTime)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number(PcbInfo.m_ullUserDirectoryTableBase, 16)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PcbInfo.m_ullKernelWaitTime)));
    m_Ui.PcbDetailTable->setItem(i++, PCB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PcbInfo.m_ullUserWaitTime)));

    i = 0;
    ProcessDetailPeb PebInfo = Info.m_Peb;
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_pPeb, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_ucInheritedAddressSpace, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_ucBeingDebugged, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_pImageBaseAddress, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_pProcessHeap, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_ulSystemReserved, 16)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PebInfo.m_ulNumberOfProcessors)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PebInfo.m_ulNumberOfHeaps)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem(QString::number(PebInfo.m_ulMaximumNumberOfHeaps)));
    m_Ui.PebDetailTable->setItem(i++, PEB_DETAIL_VALUE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)PebInfo.m_ulSessionId, 16)));
}
