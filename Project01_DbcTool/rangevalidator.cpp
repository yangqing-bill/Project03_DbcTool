#include "rangevalidator.h"
#include <QMap>

QStringList RangeValidator::validate(const Message &msg, const QMap<QString,qint64> &rawValues)
{
    QStringList outOfRange;
    for (const Signal &sig : msg.signalsList)
    {
        qint64 rawValue = rawValues.value(sig.name, 0);
        double physicalValue = rawValue * sig.factor + sig.offset;
        if (physicalValue < sig.minVal || physicalValue > sig.maxVal)
        {
            outOfRange.append(QString("%1 (值：%2 %3，允许范围：[%4,%5])")
                                  .arg(sig.name)
                                  .arg(physicalValue)
                                  .arg(sig.unit)
                                  .arg(sig.minVal)
                                  .arg(sig.maxVal));
        }
    }
    return outOfRange;
}
