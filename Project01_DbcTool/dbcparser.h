#ifndef DBCPARSER_H
#define DBCPARSER_H

#include <QString>
#include <QList>

struct Signal
{
    QString name;           //信号名称
    int startBit;           //起始位
    int length;             //长度
    bool isIntel;           //字节序标示： true = Intel (小端), false = Motorola (大端)
    double factor;          //因子
    double offset;          //偏移，其中公式：物理值 = 原始值 × Factor + Offset
    double minVal;          //最小值
    double maxVal;          //最大值
    QString unit;           //单位
};

struct Message
{
    int id;                //消息id
    QString name;           //消息名称
    int dlc;
    QList<Signal> signalsList;
};

class DbcParser
{
public:
    //静态成员函数【可以直接通过 DbcParser::parseFile(...) 调用】
    static bool parseFile(const QString &filePath,QList<Message>&messages,
                          QStringList *errors = nullptr);
};

#endif // DBCPARSER_H
