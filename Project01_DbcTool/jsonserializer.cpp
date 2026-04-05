#include "jsonserializer.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

QString JsonSerializer::serialize(const Message &msg, const QMap<QString,qint64> &rawValues)
{
    QJsonObject rootObj;
    rootObj["message_id"] = msg.id;
    rootObj["message_name"] = msg.name;

    QJsonArray signalsArray;
    for(const Signal &sig : msg.signalsList)
    {
        qint64 rawValue = rawValues.value(sig.name, 0);
        // double physicalValue = rawValue * sig.factor + sig.offset;

        double physicalValue = rawValue * sig.factor + sig.offset;
        QJsonObject signalObj;
        signalObj["name"] = sig.name;
        signalObj["physical_value"] = physicalValue;
        signalObj["unit"] = sig.unit;
        signalObj["raw_value"] = static_cast<double>(rawValue);
        signalsArray.append(signalObj);
    }

    rootObj["signals"] = signalsArray;

    QJsonDocument doc(rootObj);
    return doc.toJson(QJsonDocument::Compact);
}
