#include "MainWindow.hpp"
#include "DC.hpp"
#include "ProcessDetailWnd.hpp"
#include "ObjectTypeDetailWnd.hpp"

extern Driver* g_pDriver;

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent) 
{
    m_Ui.setupUi(this);

    ProcessInfoTableMenu = new QMenu(m_Ui.ProcessInfoTable);
    ShowProcessDetailAction = new QAction(SHOW_PROCESS_DETAIL_ACTION_NAME, m_Ui.ProcessInfoTable);
    TerminateProcessAction = new QAction(TERMINATE_PROCESS_ACTION_NAME, m_Ui.ProcessInfoTable);
    OpenProcessFolderAction = new QAction(OPEN_PROCESS_FOLDER_ACTION_NAME, m_Ui.ProcessInfoTable);

    ProcessInfoTableMenu->addAction(ShowProcessDetailAction);
    ProcessInfoTableMenu->addAction(TerminateProcessAction);
    ProcessInfoTableMenu->addAction(OpenProcessFolderAction);

    ObjectTypeInfoTableMenu = new QMenu(m_Ui.ObjectTypeInfoTable);
    ShowObjectTypeDetailAction = new QAction(SHOW_OBJECT_TYPE_DETAIL_ACTION_NAME, m_Ui.ObjectTypeInfoTable);

    ObjectTypeInfoTableMenu->addAction(ShowObjectTypeDetailAction);
    
    connect(ShowProcessDetailAction, &QAction::triggered, this, &MainWindow::ShowProcessDetailInfo);
    connect(TerminateProcessAction, &QAction::triggered, this, &MainWindow::TerminateProcess);
    connect(OpenProcessFolderAction, &QAction::triggered, this, &MainWindow::OpenProcessFolder);

    connect(m_Ui.ProcessInfoTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::ShowProcessInfoTableMenu);
    connect(m_Ui.ObjectTypeInfoTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::ShowObjectTypeInfoTableMenu);

    connect(ShowObjectTypeDetailAction, &QAction::triggered, this, &MainWindow::ShowObjectTypeDetailInfo);

    connect(m_Ui.ProcessInfoRefreshButton, &QPushButton::clicked, this, &MainWindow::RefreshProcessInfo);
    connect(m_Ui.ObjectTypeInfoRefreshButton, &QPushButton::clicked, this, &MainWindow::RefreshObjectTypeInfo);

    connect(m_Ui.ModuleInfoRefreshButton, &QPushButton::clicked, this, &MainWindow::RefreshModuleInfo);
}

MainWindow::~MainWindow() {
    delete ShowProcessDetailAction;
    delete ProcessInfoTableMenu;
}

void MainWindow::ShowProcessInfoTableMenu(const QPoint& pos) {
    ProcessInfoTableMenu->exec(QCursor::pos());
}

void MainWindow::ShowObjectTypeInfoTableMenu(const QPoint& pos) {
    ObjectTypeInfoTableMenu->exec(QCursor::pos());
}

void MainWindow::RefreshProcessInfo(bool checked) {
    m_Ui.ProcessInfoTable->clearContents();

    Vector<ProcessInfo> ProcessInfoVector = g_pDriver->CollectProcessInfo();
    ULONG Count = ProcessInfoVector.size();
    m_Ui.ProcessInfoTable->setRowCount(Count);
    for (ULONG i = 0; i < Count; i++) {
        ProcessInfo Info = ProcessInfoVector[i];
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_PEPROCESS_NAME_INDEX, new QTableWidgetItem(QString::fromStdWString(Info.m_szFullFileName).split("\\").last()));
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_PROCESS_ID_INDEX, new QTableWidgetItem(QString::number((ULONGLONG)Info.m_ProcessId)));
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_CREATE_TIME_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_CreateTime.QuadPart, 16)));
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_OWNER_PROCESS_ID_INDEX, new QTableWidgetItem(QString::number((ULONGLONG)Info.m_ullOwnerProcessId)));
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_PEPROCESS_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_pEprocess, 16)));
        m_Ui.ProcessInfoTable->setItem(i, PROCESS_INFO_FULL_FILE_NAME_INDEX, new QTableWidgetItem(QString::fromStdWString(Info.m_szFullFileName)));
    }
    ProcessInfoVector.clear();
}

void MainWindow::RefreshObjectTypeInfo(bool checked) {
    m_Ui.ObjectTypeInfoTable->clearContents();

    Vector<ObjectTypeInfo> ObjectTypeInfoVector = g_pDriver->CollectObjectTypeInfo();
    ULONG Count = ObjectTypeInfoVector.size();
    m_Ui.ObjectTypeInfoTable->setRowCount(Count);
    for (ULONG i = 0; i < Count; i++) {
        ObjectTypeInfo Info = ObjectTypeInfoVector[i];
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_INDEX_INDEX, new QTableWidgetItem(QString::number(Info.m_ucIndex)));
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_NAME_INDEX, new QTableWidgetItem(QString::fromStdWString(Info.m_szName)));
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_POBJECTTYPE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_pObjectType, 16)));
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_PDEFAULTOBJECT_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_pDefaultObject, 16)));
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_OBJECTS_NUMBER_INDEX, new QTableWidgetItem(QString::number(Info.m_ulTotalNumberOfObjects)));
        m_Ui.ObjectTypeInfoTable->setItem(i, OBJECT_TYPE_INFO_HANDLES_NUMBER_INDEX, new QTableWidgetItem(QString::number(Info.m_ulTotalNumberOfHandles)));
    }
    ObjectTypeInfoVector.clear();
}

void MainWindow::ShowProcessDetailInfo(bool checked) {
    QList<QTableWidgetItem*>  pSelectedItemList = m_Ui.ProcessInfoTable->selectedItems();

    QTableWidgetItem* pEprocessItem = pSelectedItemList[PROCESS_INFO_PEPROCESS_INDEX];
    if (!pEprocessItem) {
        return;
    }
    PVOID pEprocess = (PVOID)(pEprocessItem->text().toULongLong(NULL, 16));
    if (!pEprocess) {
        return;
    }
    ProcessDetailWnd* pDetail = new ProcessDetailWnd(pEprocess);
    pDetail->show();
}

void MainWindow::TerminateProcess(bool checked) {
    QList<QTableWidgetItem*>  pSelectedItemList = m_Ui.ProcessInfoTable->selectedItems();

    QTableWidgetItem* pEprocessItem = pSelectedItemList[PROCESS_INFO_PEPROCESS_INDEX];
    if (!pEprocessItem) {
        return;
    }
    PVOID pEprocess = (PVOID)(pEprocessItem->text().toULongLong(NULL, 16));
    if (!pEprocess) {
        return;
    }
    g_pDriver->TerminateProcess(pEprocess);
}

void MainWindow::OpenProcessFolder(bool checked) {
    QList<QTableWidgetItem*>  pSelectedItemList = m_Ui.ProcessInfoTable->selectedItems();

    QTableWidgetItem* pProcessFileItem = pSelectedItemList[PROCESS_INFO_FULL_FILE_NAME_INDEX];
    if (!pProcessFileItem) {
        return;
    }
    QString FullProcessFile = pProcessFileItem->text();
    ShellExecute(NULL, TEXT("open"), TEXT("explorer.exe"), ("/select," + FullProcessFile).toStdWString().c_str(), NULL, SW_SHOWNORMAL);
}

void MainWindow::ShowObjectTypeDetailInfo(bool checked) {
    QList<QTableWidgetItem*>  pSelectedItemList = m_Ui.ObjectTypeInfoTable->selectedItems();

    QTableWidgetItem* pObjectTypeItem = pSelectedItemList[OBJECT_TYPE_INFO_POBJECTTYPE_INDEX];
    if (!pObjectTypeItem) {
        return;
    }
    PVOID pObjectType = (PVOID)(pObjectTypeItem->text().toULongLong(NULL, 16));
    if (!pObjectType) {
        return;
    }
    ObjectTypeDetailWnd* pDetail = new ObjectTypeDetailWnd(pObjectType);
    pDetail->show();
}

void MainWindow::RefreshModuleInfo(bool checked) {
    m_Ui.ModuleInfoTable->clearContents();

    Vector<ModuleInfo> ModuleInfoVector = g_pDriver->CollectModuleInfo();
    ULONG ulCount = ModuleInfoVector.size();
    m_Ui.ModuleInfoTable->setRowCount(ulCount);
    for (ULONG i = 0; i < ulCount; i++) {
        ModuleInfo Info = ModuleInfoVector[i];
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_NAME_INDEX, new QTableWidgetItem(QString::fromStdWString(Info.m_szBaseDllName)));
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_LOAD_COUNT_INDEX, new QTableWidgetItem(QString::number(Info.m_usLoadCount)));
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_DLL_BASE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_pDllBase, 16)));
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_ENTRY_POINT_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_pEntryPoint, 16)));
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_SIZE_OF_IMAGE_INDEX, new QTableWidgetItem("0x" + QString::number((ULONGLONG)Info.m_ulSizeOfImage, 16)));
        m_Ui.ModuleInfoTable->setItem(i, MODULE_INFO_FILE_INDEX, new QTableWidgetItem(QString::fromStdWString(Info.m_szFullDllName)));
    }
    ModuleInfoVector.clear();

}
