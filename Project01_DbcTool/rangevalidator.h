#ifndef RANGEVALIDATOR_H
#define RANGEVALIDATOR_H

#include <QStringList>
#include "dbcparser.h"

class RangeValidator
{
public:
    // 校验逻辑
    static QStringList validate(const Message &msg, const QMap<QString,qint64> &rawValues);

};

#endif // RANGEVALIDATOR_H
