#ifndef UTILS_H
#define UTILS_H

#include <QByteArray>
#include <QString>

//工具函数
class Utils
{
public:
    // 将字节数组转换为十六进制字符串，例如 "01 2A FF"
    static QString byteArrayToHexString(const QByteArray &data);

    // 检查 double 是否为零（或接近零），避免除零错误
    static bool isZero(double value);

    // 物理值转原始值（公式：raw = (physical - offset) / factor）
    static qint64 physicalToRaw(double physicalValue,double factor,double offset);

};

#endif // UTILS_H
