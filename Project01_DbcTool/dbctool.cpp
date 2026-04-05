#include "dbctool.h"
#include "ui_dbctool.h"
#include "canframebuilder.h"
#include "logger.h"
#include "Constants.h"
#include "utils.h"
#include "rangevalidator.h"
#include "jsonserializer.h"
#include "tabmanager.h"
#include <QAction>
#include <QMenuBar>
#include <QMap>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QLabel>
#include <QDoubleSpinBox>
#include "CanMessage/canframe.qpb.h"
#include <QProtobufSerializer>

DbcTool::DbcTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DbcTool)
{
    ui->setupUi(this);
    initMenuBar();
    initLayout();
    initTree();
    initTable();
    initWebSocket();

    m_logger = new Logger(ui->logTextEdit,this);
}

DbcTool::~DbcTool()
{
    delete ui;
}

void DbcTool::initMenuBar()             // 菜单功能初始化
{
    QMenuBar *menubar = menuBar();
    QMenu *fileMenu = menubar->addMenu("文件");
    QAction *actionOpen = new QAction("打开DBC文件",this);
    QAction *actionQuit = new QAction("退出",this);

    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionQuit);
    connect(actionOpen,&QAction::triggered,this,&DbcTool::onOpenDbcFile);
    connect(actionQuit,&QAction::triggered,this,&DbcTool::close);
}

void DbcTool::initLayout()              // 窗口布局
{
    this->resize(Constants::MAIN_WINDOW_WIDTH,Constants::MAIN_WINDOW_HEIGHT);
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,ui->central);
    mainSplitter->addWidget(ui->tree);
    initRightLayout();                  // 初始化右侧区域

    mainSplitter->addWidget(ui->rightContainer);
    mainSplitter->setStretchFactor(0,2);
    mainSplitter->setStretchFactor(1,3);

    QVBoxLayout *centralLayout = new QVBoxLayout(ui->central);
    centralLayout->addWidget(mainSplitter);
    ui->central->setLayout(centralLayout);
}

void DbcTool::initRightLayout()         // 右侧区域初始化
{
    QVBoxLayout *rightLayout = new QVBoxLayout(ui->rightContainer);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->addWidget(ui->table);

    ui->m_tabWidget->setTabsClosable(true);     // 在每个标签页的标题栏右侧显示一个关闭按钮
    rightLayout->addWidget(ui->m_tabWidget);

    ui->m_tabWidget->removeTab(1);
    ui->m_tabWidget->removeTab(0);
    m_tabManager = new TabManager(ui->m_tabWidget,this);
    connect(m_tabManager,&TabManager::currentChanged,this,[=](int msgIndex)
            {
                if (msgIndex < 0 || msgIndex >= m_messages.size())
                    return;
                const Message &msg = m_messages.at(msgIndex);
                ui->table->clearContents();
                ui->table->setRowCount(0);
                for(const Signal &sig : msg.signalsList)
                {
                    int row = ui->table->rowCount();
                    ui->table->insertRow(row);
                    ui->table->setItem(row,0,new QTableWidgetItem(sig.name));
                    ui->table->setItem(row,1,new QTableWidgetItem(QString::number(sig.startBit)));
                    ui->table->setItem(row,2,new QTableWidgetItem(QString::number(sig.length)));
                    ui->table->setItem(row,3,new QTableWidgetItem(sig.isIntel ? "Intel" : "Motorola"));
                    ui->table->setItem(row,4,new QTableWidgetItem(QString::number(sig.factor)));
                    ui->table->setItem(row,5,new QTableWidgetItem(QString::number(sig.offset)));
                    ui->table->setItem(row,6,new QTableWidgetItem(QString::number(sig.minVal)));
                    ui->table->setItem(row,7,new QTableWidgetItem(QString::number(sig.maxVal)));
                    ui->table->setItem(row,8,new QTableWidgetItem(sig.unit));
                }
                ui->table->resizeColumnsToContents();
                ui->packResultEdit->clear();
            });
}

void DbcTool::initTree()        //树控件初始化
{
    QStringList treeHeaders = {Constants::treeHeaders};
    ui->tree->setHeaderLabels(treeHeaders);
    for(int i = 0; i < ui->tree->columnCount(); ++i)
    {
        ui->tree->header()->setSectionResizeMode(i,QHeaderView::Stretch);
    }
    connect(ui->tree, &QTreeWidget::itemClicked, this, &DbcTool::onMessageSelected);
    connect(ui->onSendButton, &QPushButton::clicked, this, &DbcTool::packPreview);
}

void DbcTool::initTable()           //table控件初始化
{
    QStringList tableHeaders = {Constants::tableHeaders};
    ui->table->setColumnCount(tableHeaders.size());     //设置列数
    ui->table->setHorizontalHeaderLabels(tableHeaders); //设置水平表头
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //列宽自动适应内容
}

void DbcTool::initWebSocket()       //网络初始化
{
    qDebug() << "initWebSocket";
    ui->lineEditServerAddress->setText(Constants::DEFAULT_WEBSOCKET_URL);
    m_wsClient = new CanWebSocketClient(this);
    connect(m_wsClient, &CanWebSocketClient::connected, this, &DbcTool::onWebSocketConnected);
    connect(m_wsClient, &CanWebSocketClient::disconnected, this, &DbcTool::onWebSocketDisconnected);
    connect(m_wsClient, &CanWebSocketClient::errorOccurred, this, &DbcTool::onWebSocketErrorOccurred);

    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &DbcTool::serverConnect);
    connect(ui->pushButtonSend, &QPushButton::clicked, this, &DbcTool::serverSend);

    ui->statusIndicator->setFixedSize(12, 12);   // 根据实际大小调整
    ui->statusIndicator->setStyleSheet("QLabel { background-color: #F44336; border-radius: 6px; }");
    updateConnectionStatus(false);
}


void DbcTool::onOpenDbcFile()       //阶段02：打开DBC文件菜单动作
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择 DBC 文件",QDir::homePath(),"DBC Files (*.dbc)");
    if(fileName.isEmpty())
        return;

    m_tabManager->clearAllTabs();
    ui->packResultEdit->clear();

    QList<Message> messages;
    QStringList errors;
    bool paresOk = DbcParser::parseFile(fileName,messages,&errors);

    if(!errors.isEmpty())
    {
        for(const QString &err : errors)
            m_logger->appendLog("解析错误：" + err);
    }

    if(messages.isEmpty())
    {
        QMessageBox::warning(this,"解析错误", "无法解析该 DBC 文件，请检查格式。");
        return;
    }

    if (!errors.isEmpty())
    {
        QMessageBox::warning(this, "部分解析失败",
                             QString("解析完成，但存在 %1 个错误。\n详细信息已写入日志面板。").arg(errors.size()));
    }

    m_messages = messages;
    ui->tree->clear();

    for(int i = 0; i < m_messages.size(); ++i)
    {
        const Message &msg = m_messages[i];

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->tree);
        // item->setText(0,QString::number(msg.id));        //十进制
        item->setText(0,"0x" + QString::number(msg.id,16).toUpper());   //十六进制（大写）
        item->setText(1,msg.name);
        // qDebug() << "添加节点：" << msg.id << msg.name;

        int index = i;
        item->setData(0,Qt::UserRole,index);    //存储数据
    }

    ui->tree->expandAll();
    ui->table->clearContents();
    ui->table->setRowCount(0);
    statusBar()->showMessage("已加载: " + fileName);
}

void DbcTool::onMessageSelected(QTreeWidgetItem *item, int column)  //阶段02：树控件点击事件
{
    Q_UNUSED(column);   //column未使用
    int index = item->data(0, Qt::UserRole).toInt();    //提取存储信息
    if (index < 0 || index >= m_messages.size())
        return;

    m_tabManager->openMessage(index,m_messages.at(index));
}

void DbcTool::packPreview()
{
    int msgIndex = m_tabManager->currentMessageIndex();
    if (msgIndex == -1) {
        QMessageBox::warning(this, "提示", "请先打开一个消息标签页");
        return;
    }
    const QMap<QString, qint64> *rawValues = m_tabManager->currentRawValues();
    if(!rawValues)
    {
        QMessageBox::warning(this,"错误", "当前标签页对应的消息无效");
        return;
    }

    const Message &msg = m_messages.at(msgIndex);
    QByteArray frame = CanFrameBuilder::packMessage(msg,*rawValues);
    ui->packResultEdit->setPlainText(Utils::byteArrayToHexString(frame));
}

void DbcTool::serverConnect()
{
    if(!m_wsClient->isConnected())
    {
        QString address = ui->lineEditServerAddress->text().trimmed();
        if(address.isEmpty())
        {
            QMessageBox::warning(this,"地址无效","请输入WebSocket服务器地址");
            return;
        }
        m_wsClient->connectToServer(address);
        ui->pushButtonConnect->setEnabled(false);
        statusBar()->showMessage("正在连接");
    }
    else
    {
        m_wsClient->disconnect();
    }
}

void DbcTool::serverSend()
{
    static int callCount = 0;
    qDebug() << "Send called #" << ++callCount;
    int msgIndex = m_tabManager->currentMessageIndex();
    if(msgIndex == -1)
    {
        QMessageBox::warning(this,"提示","请先打开一个消息标签页");
        return;
    }
    const QMap<QString, qint64> *rawValues = m_tabManager->currentRawValues();
    if(!rawValues)
    {
        QMessageBox::warning(this,"错误", "当前标签页对应的消息无效");
        return;
    }
    const Message &msg = m_messages.at(msgIndex);

    QStringList outOfRangeSignals = RangeValidator::validate(msg, *rawValues);
    if (!outOfRangeSignals.isEmpty())
    {
        QMessageBox::warning(this, "范围校验错误", outOfRangeSignals.join("\n"));
        return;
    }

    if(!m_wsClient->isConnected())
    {
        QMessageBox::warning(this,"提示","请先连接WebSocket 服务器");
        return;
    }

    if(ui->useProtobufCheckBox->isChecked())
    {
        CanMessage::CanFrame frame;
        frame.setMessageId(msg.id);
        frame.setMessageName(msg.name);

        QList<CanMessage::Signal> signalsList;
        for(const Signal &sig : msg.signalsList)
        {
            qint64 rawValue = rawValues->value(sig.name,0);
            double physicalValue = rawValue * sig.factor + sig.offset;

            CanMessage::Signal signal;
            signal.setName(sig.name);
            signal.setPhysicalValue(physicalValue);
            signal.setUnit(sig.unit);
            signal.setRawValue(rawValue);
            signalsList.append(signal);
        }
        frame.setSignals_(signalsList);
        QProtobufSerializer serializer;
        QByteArray binaryData = serializer.serialize(&frame);
        m_wsClient->sendBinaryMessage(binaryData);

        m_logger->appendLog(QString("发送 Protobuf [ID:0x%1] [Name:%2] 信号数:%3")
                                .arg(QString::number(msg.id,16).toUpper())
                                .arg(msg.name)
                                .arg(msg.signalsList.size()));
    }
    else
    {
        QString jsonString = JsonSerializer::serialize(msg,*rawValues);

        m_wsClient->sendTextMessage(jsonString);

        QString logMsg = QString("发送【ID:%1][Name:%2] 信号数量：%3")
                             .arg(QString::number(msg.id,16).toUpper())
                             .arg(msg.name)
                             .arg(msg.signalsList.size());
        m_logger->appendLog(logMsg);
    }
    statusBar()->showMessage("已发送JSON数据");
}

void DbcTool::onWebSocketConnected()
{
    updateConnectionStatus(true);
    m_logger->appendLog("WebSocket已连接到" + m_wsClient->serverAddress());
}

void DbcTool::onWebSocketDisconnected()
{
    updateConnectionStatus(false);
    m_logger->appendLog("WebSocket已断开");
}

void DbcTool::onWebSocketErrorOccurred(const QString &errorString)
{
    QMessageBox::critical(this,"WebSocket 错误",errorString);
    statusBar()->showMessage("连接错误： "+ errorString);
    updateConnectionStatus(false);
    m_logger->appendLog("WebSocket错误: " + errorString);
}

void DbcTool::updateConnectionStatus(bool connected)
{
    if(connected)
    {
        ui->pushButtonConnect->setText("断开");
        ui->pushButtonConnect->setEnabled(true);
        statusBar()->showMessage("WebSocket 已连接： " + m_wsClient->serverAddress());

        ui->statusIndicator->setStyleSheet(
            "QLabel { background-color: #4CAF50; "
            "border-radius: 5px;"
            "border: 1px solid black;}");
        ui->statusIndicator->setToolTip("已连接");
    }
    else
    {
        ui->pushButtonConnect->setText("连接");
        ui->pushButtonConnect->setEnabled(true);
        statusBar()->showMessage("WebSocket 已断开");

        ui->statusIndicator->setStyleSheet(
            "QLabel{ background-color: #F44336; "
            "border-radius: 5px;"
            "border: 1px solid black;}");
        ui->statusIndicator->setToolTip("未连接");
    }
}


void DbcTool::on_clearLogButton_clicked()           // 日志清空
{
    m_logger->clear();
}
