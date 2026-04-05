#include "tabmanager.h"
#include "SignalInputPanel.h"
#include <QTabWidget>
#include <QScrollArea>
#include <QVBoxLayout>

TabManager::TabManager(QTabWidget *tabWidget, QObject *parent)
    : QObject(parent), m_tabWidget(tabWidget)
{
    // 连接标签页切换信号
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TabManager::onTabBarClicked);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &TabManager::onTabCloseRequested);
}

void TabManager::rebuildIndexMap()
{
    m_tabIndexToMessageIndex.clear();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *tab = m_tabWidget->widget(i);
        if (tab) {
            int msgIndex = tab->property("msgIndex").toInt();
            m_tabIndexToMessageIndex[i] = msgIndex;
        }
    }
}

void TabManager::openMessage(int msgIndex, const Message &msg)
{
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *tab = m_tabWidget->widget(i);
        if (tab && tab->property("msgIndex").toInt() == msgIndex) {
            m_tabWidget->setCurrentIndex(i);
            emit currentChanged(msgIndex);
            return;
        }
    }

    // 创建标签页内容容器
    QWidget *tabPage = new QWidget;
    tabPage->setProperty("msgIndex", msgIndex);

    QVBoxLayout *pageLayout = new QVBoxLayout(tabPage);
    pageLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *container = new QWidget;
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setAlignment(Qt::AlignTop);
    container->setLayout(containerLayout);

    scrollArea->setWidget(container);
    pageLayout->addWidget(scrollArea);


    // 先添加标签页，获取索引
    QString tabTitle = QString("0x%1 %2").arg(msg.id, 0, 16).toUpper().arg(msg.name);
    int tabIndex = m_tabWidget->addTab(tabPage, tabTitle);

    // 创建 rawValues 映射并存储
    m_tabRawValues[tabPage] = QMap<QString, qint64>();

    // 创建输入面板
    m_tabRawValues[tabPage] = QMap<QString, qint64>();
    SignalInputPanel *panel = new SignalInputPanel(msg, m_tabRawValues[tabPage], container);
    containerLayout->addWidget(panel);

    // 存储映射
    m_tabIndexToMessageIndex[tabIndex] = msgIndex;

    // 切换到新标签页
    m_tabWidget->setCurrentIndex(tabIndex);
    emit currentChanged(msgIndex);
}

int TabManager::currentMessageIndex() const
{
    int idx = m_tabWidget->currentIndex();
    if (idx == -1)
        return -1;
    return m_tabIndexToMessageIndex.value(idx, -1);
}

const QMap<QString, qint64>* TabManager::currentRawValues() const
{
    QWidget *current = m_tabWidget->currentWidget();
    if (!current) return nullptr;
    auto it = m_tabRawValues.find(current);
    if (it == m_tabRawValues.end()) return nullptr;
    return &(it.value());
}

const QMap<QString, qint64>& TabManager::rawValuesAt(int tabIndex) const
{
    QWidget *tab = m_tabWidget->widget(tabIndex);
    static QMap<QString, qint64> empty;
    if (!tab) return empty;
    auto it = m_tabRawValues.find(tab);
    if (it == m_tabRawValues.end()) return empty;
    return it.value();
}

void TabManager::onTabBarClicked(int index)
{
    if (index != -1 && m_tabIndexToMessageIndex.contains(index))
    {
        emit currentChanged(m_tabIndexToMessageIndex[index]);
    }
}

void TabManager::closeTab(int index)
{
    QWidget *widget = m_tabWidget->widget(index);
    if (!widget) return;

    // 移除 rawValues 映射
    m_tabRawValues.remove(widget);
    // 移除索引映射
    m_tabIndexToMessageIndex.remove(index);
    // 删除标签页
    m_tabWidget->removeTab(index);
    widget->deleteLater();

    // 关键：重建索引映射，因为关闭后后面标签页的索引会前移
    rebuildIndexMap();
}

void TabManager::onTabCloseRequested(int index)
{
    closeTab(index);
}

void TabManager::clearAllTabs()
{
    for (int i = m_tabWidget->count() - 1; i >= 0; --i)
    {
        closeTab(i);
    }
}
