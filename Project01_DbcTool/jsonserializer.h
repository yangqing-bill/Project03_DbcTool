#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <QMap>
#include <QString>
#include "dbcparser.h"

class JsonSerializer
{
public:
    // 将消息及其原始值序列化为符合题目要求的 JSON 字符串
    static QString serialize(const Message &msg, const QMap<QString,qint64> &rawValues);
};

#endif // JSONSERIALIZER_H
