#ifndef SIGNALINPUTPANEL_H
#define SIGNALINPUTPANEL_H

#include <QWidget>
#include <QMap>
#include "dbcparser.h"

class SignalInputPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SignalInputPanel(const Message &msg, QMap<QString, qint64> &rawValues, QWidget *parent = nullptr);

    // 获取该面板的原始值映射（只读）
    const QMap<QString,qint64>& rawValues() const
    {
        return m_rawValues;
    }

    // 可选：范围校验，返回错误信息列表
    QStringList validate() const;

private:
    void createInputs();

    const Message &m_msg;
    QMap<QString,qint64> &m_rawValues;
};

#endif // SIGNALINPUTPANEL_H
