#ifndef CANWEBSOCKETCLIENT_H
#define CANWEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QUrl>

class CanWebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit CanWebSocketClient(QObject *parent = nullptr);
    ~CanWebSocketClient();

    bool isConnected() const { return m_connected; }            // 返回当前连接状态
    QString serverAddress() const { return m_serverAddress; }   // 返回当前连接服务器地址

public slots:
    void connectToServer(const QString &url);                   // 连接服务器
    void disconnect();                                          // 主动断开连接
    void sendTextMessage(const QString &message);               // 发送文本消息
    void sendBinaryMessage(const QByteArray &data);

signals:
    void connected();                                           // 连接成功发送信号
    void disconnected();                                        // 连接断开发送信号
    void errorOccurred(const QString &errorString);             // 发送错误时发送信号

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QWebSocket *m_webSocket;                                    // 网络端对象
    bool m_connected;                                           // 连接状态：是否连接
    QString m_serverAddress;                                    // 当前连接的服务端地址
};

#endif // CANWEBSOCKETCLIENT_H
