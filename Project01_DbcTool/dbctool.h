#ifndef DBCTOOL_H
#define DBCTOOL_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QScrollArea>
#include <QVBoxLayout>
#include "dbcparser.h"
#include "canwebsocketclient.h"

class Logger;
class TabManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class DbcTool;
}
QT_END_NAMESPACE

class DbcTool : public QMainWindow
{
    Q_OBJECT

public:
    DbcTool(QWidget *parent = nullptr);
    ~DbcTool();

private slots:
    void onOpenDbcFile();                                       // 打开DBC文件菜单动作
    void onMessageSelected(QTreeWidgetItem *item, int column);  // 树控件点击事件
    void onWebSocketConnected();                                // 网络已连接
    void onWebSocketDisconnected();                             // 网络未连接
    void onWebSocketErrorOccurred(const QString &errorString);  // 网络连接错误
    void serverConnect();                                       // 连接服务端
    void serverSend();                                          // 发送按钮
    void packPreview();                                         // 打包预览按钮
    void on_clearLogButton_clicked();                           // 日志清空

private:
    Ui::DbcTool *ui;
    void initMenuBar();                                         // 菜单功能初始化
    void initLayout();                                          // 主窗口布局初始化
    void initRightLayout();                                     // 窗口01布局初始化
    void initTree();                                            // 树控件初始化
    void initTable();                                           // table控件初始化
    void initWebSocket();                                       // 网络初始化

    QList<Message> m_messages;                                  // 设置容器
    CanWebSocketClient *m_wsClient;                             // 网络端
    void updateConnectionStatus(bool);                          // 更新界面状态
    Logger *m_logger;
    TabManager *m_tabManager;
};

#endif // DBCTOOL_H
