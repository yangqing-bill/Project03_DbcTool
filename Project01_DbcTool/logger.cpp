#include "logger.h"
#include <QDateTime>
#include <QTextCursor>
#include "Constants.h"

Logger::Logger(QTextEdit *logWidget,QObject *parent)
    : QObject{parent},m_logWidget(logWidget)
{}

void Logger::appendLog(const QString &message)
{
    if(!m_logWidget)
        return;

    QString timestamp = QDateTime::currentDateTime().toString(Constants::LOG_TIMESTAMP_FORMAT);   //获取当前时间
    QString logLine = QString(Constants::LOG_LINE_PATTERN).arg(timestamp,message);                        //格式化日志
    m_logWidget->append(logLine);                                                       //追加到日志控件
    m_logWidget->moveCursor(QTextCursor::End);                                          //自动滚动至底部
}

void Logger::clear()
{
    if(m_logWidget)
        m_logWidget->clear();
}
