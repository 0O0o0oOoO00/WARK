#pragma once

#include <Windows.h>
#include "MainWindowUI.h"
#include <QtWidgets/QWidget>
#include <QMenu>
#include <QAction>

#define PROCESS_INFO_PEPROCESS_NAME_INDEX 0
#define PROCESS_INFO_PROCESS_ID_INDEX 1
#define PROCESS_INFO_CREATE_TIME_INDEX 2
#define PROCESS_INFO_OWNER_PROCESS_ID_INDEX 3
#define PROCESS_INFO_PEPROCESS_INDEX 4
#define PROCESS_INFO_FULL_FILE_NAME_INDEX 5

#define OBJECT_TYPE_INFO_INDEX_INDEX 0
#define OBJECT_TYPE_INFO_NAME_INDEX 1
#define OBJECT_TYPE_INFO_POBJECTTYPE_INDEX 2
#define OBJECT_TYPE_INFO_PDEFAULTOBJECT_INDEX 3
#define OBJECT_TYPE_INFO_OBJECTS_NUMBER_INDEX 4
#define OBJECT_TYPE_INFO_HANDLES_NUMBER_INDEX 5

#define SHOW_PROCESS_DETAIL_ACTION_NAME "Detail"
#define TERMINATE_PROCESS_ACTION_NAME "Terminate"
#define OPEN_PROCESS_FOLDER_ACTION_NAME "OpenFolder"

#define SHOW_OBJECT_TYPE_DETAIL_ACTION_NAME "Detail"

#define MODULE_INFO_NAME_INDEX 0
#define MODULE_INFO_LOAD_COUNT_INDEX 1
#define MODULE_INFO_DLL_BASE_INDEX 2
#define MODULE_INFO_ENTRY_POINT_INDEX 3
#define MODULE_INFO_SIZE_OF_IMAGE_INDEX 4
#define MODULE_INFO_FILE_INDEX 5

class MainWindow : public QWidget {
    Q_OBJECT

private:
    Ui::MainWindowClass m_Ui;
    QMenu* ProcessInfoTableMenu;
    QAction* ShowProcessDetailAction;
    QAction* TerminateProcessAction;
    QAction* OpenProcessFolderAction;

    QMenu* ObjectTypeInfoTableMenu;
    QAction* ShowObjectTypeDetailAction;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void RefreshProcessInfo(bool checked = false);
    void RefreshObjectTypeInfo(bool checked = false);
    void ShowProcessInfoTableMenu(const QPoint& pos);
    void ShowProcessDetailInfo(bool checked);
    void TerminateProcess(bool checked);
    void OpenProcessFolder(bool checked);
    void ShowObjectTypeDetailInfo(bool checked);
    void ShowObjectTypeInfoTableMenu(const QPoint& pos);
    void RefreshModuleInfo(bool checked = false);
};
