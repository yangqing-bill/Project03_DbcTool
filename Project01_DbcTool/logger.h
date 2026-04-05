#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QTextEdit>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QTextEdit *logWidget,QObject *parent = nullptr);

public slots:
    void appendLog(const QString &message);
    void clear();

private:
    QTextEdit *m_logWidget;
};

#endif // LOGGER_H
