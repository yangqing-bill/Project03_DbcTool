#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QStringList>

//纯常量
namespace Constants {

// 窗口尺寸
constexpr int MAIN_WINDOW_WIDTH = 670;
constexpr int MAIN_WINDOW_HEIGHT = 630;

// 树控件表头
const QStringList treeHeaders = {"Message ID" , "Message Name"};

// 表格控件表头
const QStringList tableHeaders = {"信号名", "起始位", "长度", "字节序", "因子", "偏移量", "最小值", "最大值", "单位"};

// 日志格式：
const QString LOG_TIMESTAMP_FORMAT = "yyyy-MM-dd hh:mm:ss";
const QString LOG_LINE_PATTERN = "[%1] %2";

// UI控件宽度
constexpr int LABEL_NAME_WIDTH = 120;
constexpr int LABEL_UNIT_WIDTH = 60;
constexpr int LABEL_RAW_WIDTH = 100;
constexpr int LABEL_RANGE_WIDTH = 100;

//默认WebSocket地址
const QString DEFAULT_WEBSOCKET_URL = "ws://localhost:8080";

}

#endif // CONSTANTS_H
