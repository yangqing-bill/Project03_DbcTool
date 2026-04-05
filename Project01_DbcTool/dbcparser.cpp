#include "dbcparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

bool DbcParser::parseFile(const QString &filePath, QList<Message> &messages,QStringList *errors)
{
    QFile file(filePath);                                       //地址判断
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))      //按位或（|）对两个整数的每一位分别进行或运算，结果是整数
        return false;

    QTextStream in(&file);

    Message currentMessage;     //暂存当前正在解析的消息
    bool inMessage = false;     // 标记是否正在处理一个 Message
    bool hasError = false;      //错误标志
    int lineNumber = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        lineNumber++;
        QString trimmed = line.trimmed();     //逐行读取，去除两端空白
        if (line.isEmpty()) continue;

        // 跳过非关键行
        if(trimmed.startsWith("NS_")        || trimmed.startsWith("BS_")         || trimmed.startsWith("BU_")    ||
            trimmed.startsWith("VERSION")   || trimmed.startsWith("CM_")         || trimmed.startsWith("BA_")    ||
            trimmed.startsWith("VAL_")      || trimmed.startsWith("CAT_")        || trimmed.startsWith("FILTER") ||
            trimmed.startsWith("BO_TX_BU_") || trimmed.startsWith("SG_MUL_VAL_"))
            continue;

        if(trimmed.startsWith("BO_"))
        {
            if(inMessage && currentMessage.signalsList.size() > 0)
            {
                messages.append(currentMessage);
            }

            currentMessage = Message();
            inMessage = true;

            // 解析 BO_ 行
            QStringList parts =trimmed.split(' ', Qt::SkipEmptyParts);
            if(parts.size() < 4)
            {
                if (errors)
                    errors->append(QString("行 %1: 无效的 BO_ 行: %2").arg(lineNumber).arg(trimmed));
                hasError = true;
                inMessage = false;
                continue;
            }

            bool ok;
            int id = parts[1].toInt(&ok);
            if(!ok)
            {
                if (errors)
                    errors->append(QString("行 %1: 无效的消息 ID: %2").arg(lineNumber).arg(trimmed));
                hasError = true;
                inMessage = false;
                continue;
            }

            QString name = parts[2];
            if (name.endsWith(':')) {
                name.chop(1);               // 去掉冒号
            }

            int dlc = parts[3].toInt(&ok);
            if (!ok) dlc = 0;               // 如果解析失败，默认为0

            currentMessage.id = id;
            currentMessage.name = name;
            currentMessage.dlc = dlc;
        }
        else if (trimmed.startsWith("SG_") && inMessage)
        {
            Signal sig;
            QRegularExpression regex(               //正则表达式
                R"(SG_\s+(\S+)\s*:\s*(\d+)\|(\d+)@([01][+-])\s*\(([-\d.]+),([-\d.]+)\)\s*\[([-\d.]+)\|([-\d.]+)\]\s*\"([^\"]*)\")");

            if(!regex.isValid())
                return false;

            //SG_ VehicleSpeed : 0|16@1+ (0.01,0) [0|300] "km/h"  Vector__XXX
            QRegularExpressionMatch match = regex.match(trimmed);
            if (match.hasMatch())
            {
                sig.name = match.captured(1);
                sig.startBit = match.captured(2).toInt();
                sig.length = match.captured(3).toInt();

                QString endianStr = match.captured(4);      // 字节序：@1+/@0+表示Intel（小端），@1-/@0-表示Motorola（大端）
                sig.isIntel = endianStr.contains('+');      //仅支持intel格式

                sig.factor = match.captured(5).toDouble();
                sig.offset = match.captured(6).toDouble();
                sig.minVal = match.captured(7).toDouble();
                sig.maxVal = match.captured(8).toDouble();
                sig.unit = match.captured(9);
                currentMessage.signalsList.append(sig);
            }
            else
            {
                if (errors)
                    errors->append(QString("行 %1: 无法解析 SG_ 行: %2").arg(lineNumber).arg(trimmed));
                hasError = true;   // 信号格式错误
            }
        }
    }

    if (inMessage && currentMessage.signalsList.size() > 0)    //防止文件只有一条消息
    {
        messages.append(currentMessage);
    }
    file.close();

    if (messages.isEmpty())
        return false;
    return true;
}

