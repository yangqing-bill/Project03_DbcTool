#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QObject>
#include <QMap>
#include <QTabWidget>
#include "dbcparser.h"

class QWidget;

class TabManager : public QObject
{
    Q_OBJECT
public:
    explicit TabManager(QTabWidget *tabWidget, QObject *parent = nullptr);

    // 打开消息（如果已存在则切换，否则创建新标签页）
    void openMessage(int msgIndex, const Message &msg);

    // 关闭指定索引的标签页
    void closeTab(int index);

    // 清空所有标签页
    void clearAllTabs();

    // 获取当前活动标签页的信息
    int currentMessageIndex() const;   // 返回 -1 如果没有标签页
    const QMap<QString, qint64>* currentRawValues() const;

    // 获取指定标签页的 rawValues（供外部发送时使用）
    const QMap<QString, qint64>& rawValuesAt(int tabIndex) const;

    void rebuildIndexMap();

signals:
    void currentChanged(int msgIndex);   // 当切换标签页时发出，供主窗口更新表格

private slots:
    void onTabBarClicked(int index);     // 处理用户点击标签页切换
    void onTabCloseRequested(int index); // 处理关闭按钮

private:
    QTabWidget *m_tabWidget;
    QMap<int, int> m_tabIndexToMessageIndex;      // 标签页索引 -> 消息在 m_messages 中的索引
    QMap<QWidget*, QMap<QString, qint64>> m_tabRawValues; // 标签页索引 -> 原始值映射

};

#endif // TABMANAGER_H
