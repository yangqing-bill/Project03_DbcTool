#include "canframebuilder.h"
#include <QDebug>

void CanFrameBuilder::setBitsIntel(QByteArray &data, int startBit, int length, qint64 value)
{
    // 1. 确保数据数组至少有 8 字节（CAN 帧固定为 8）
    if(data.size() < 8)
        data.resize(8);

    // 2. 截断value到有效位宽[11111111]/[xxxxxxxxxx]
    qint64 mask = (length >=64) ? ~0ll : ((1LL << length) - 1 );
    value &= mask;

    // 3. 初始化起始位和长度
    int remaining = length;
    int bitPos = startBit;

    while(remaining > 0)
    {
        int byteIndex = bitPos / 8;                             // 当前字节索引
        int bitOffset = bitPos % 8;                             // 当前字节内起始位偏移
        int bitsThisByte = qMin(remaining, 8 - bitOffset);      // 当前字节能写的位数

        // 4. 从 value 中取出待写入的 bitsThisByte 位（低 bitsThisByte 位）
        quint8 byteValue = static_cast<quint8>(value&((1ULL << bitsThisByte) - 1 ));

        // 5. 提取缓冲区原样，缓存临时变量
        quint8 currentByte = static_cast<quint8>(data[byteIndex]);

        // 6. 对应写入位字节清零
        quint8 maskByte = ((1 << bitsThisByte) - 1) << bitOffset;
        currentByte &= ~maskByte;

        // 7. 写入新值
        currentByte |=(byteValue << bitOffset);
        data[byteIndex] = static_cast<char>(currentByte);

        // 8. 更新剩余位数和位置
        remaining -= bitsThisByte;                              //剩余位数减少
        bitPos += bitsThisByte;                                 //全局写入位置向后移动
        value >>= bitsThisByte;
    }
}

void CanFrameBuilder::setBitsMotorola(QByteArray &data, int startBit, int length, qint64 value)
{
    if(data.size() < 8)
        data.resize(8);

    quint64 uvalue = static_cast<quint64>(value);
    qint64 mask = (length >=64) ? ~0ll : ((1LL << length) - 1 );
    uvalue &= mask;

    int remaining = length;
    int bitPos = startBit;          // Motorola起始位是MSB

    while (remaining > 0)
    {
        int byteIndex = bitPos / 8;
        int bitOffset = bitPos % 8;
        // Motorola在一个字节内是从高位向低位填充，所以当前字节可写的位数是 bitOffset + 1
        int bitsThisByte = qMin(remaining, bitOffset + 1);

        // 取出 value 中最高位的 bitsThisByte 位（因为Motorola高位在前）
        int shift = remaining - bitsThisByte;   // 需要右移的位数，把最高位移到最低位
        quint8 byteValue = static_cast<quint8>((value >> shift) & ((1ULL << bitsThisByte) - 1));

        // 在当前字节内，需要将 byteValue 放到从 bitOffset 开始向低位填充的位置
        // 实际偏移 = bitOffset - bitsThisByte + 1
        int byteShift = bitOffset - bitsThisByte + 1;
        byteValue <<= byteShift;

        quint8 currentByte = static_cast<quint8>(data[byteIndex]);
        quint8 maskByte = ((1 << bitsThisByte) - 1) << byteShift;
        currentByte &= ~maskByte;
        currentByte |= byteValue;
        data[byteIndex] = static_cast<char>(currentByte);

        // 更新剩余位数和位指针（Motorola向低位移动）
        remaining -= bitsThisByte;
        bitPos -= bitsThisByte;
        // 注意：value 不需要右移，因为我们每次直接从原始 value 中取高位

        if (bitPos < 0 && remaining > 0) {
            qWarning() << "Motorola packing error: startBit too small for length";
            break;
        }
    }
}

QByteArray CanFrameBuilder::packMessage(const Message &msg, const QMap<QString, qint64> &rawValues)
{
    QByteArray data(8,0);

    for(const Signal &sig : msg.signalsList)
    {
        qint64 rawValue = rawValues.value(sig.name,0);
        if(sig.isIntel)
        {
            setBitsIntel(data,sig.startBit,sig.length,rawValue);
        }
        else
        {
            setBitsMotorola(data,sig.startBit,sig.length,rawValue);
        }
    }
    int dlc = qBound(0, msg.dlc, 8);
    return data.left(dlc);
}
