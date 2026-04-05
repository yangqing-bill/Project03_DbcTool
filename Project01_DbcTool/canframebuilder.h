#ifndef CANFRAMEBUILDER_H
#define CANFRAMEBUILDER_H

#include<QByteArray>
#include<QMap>
#include"dbcparser.h"

class CanFrameBuilder
{
public:
    // 主打包函数：将消息的信号原始值打包成 8 字节数据
    static QByteArray packMessage(const Message &msg,const QMap<QString,qint64> &rawValues);

private:
    // 辅助函数：将一个整数值按位写入字节数组（Intel 小端模式）
    static void setBitsIntel(QByteArray &data, int startBit, int length, qint64 value);

    // Motorola 打包函数
    static void setBitsMotorola(QByteArray &data, int startBit, int length, qint64 value);

};

#endif // CANFRAMEBUILDER_H
