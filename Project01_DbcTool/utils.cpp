#include "utils.h"
#include <QtMath>

QString Utils::byteArrayToHexString(const QByteArray &data)
{
    QString result;
    for(int i = 0; i < data.size(); ++i)
    {
        result += QString("%1 ").arg(static_cast<quint8>(data[i]),2,16,QChar('0')).toUpper();
    }
    return result.trimmed();
}

bool Utils::isZero(double value)
{
    return qFuzzyIsNull(value);
}


qint64 Utils::physicalToRaw(double physicalValue,double factor,double offset)
{
    if(isZero(factor))
        return 0;

    double rawDouble = (physicalValue - offset) / factor;
    return qRound64(rawDouble);
}

