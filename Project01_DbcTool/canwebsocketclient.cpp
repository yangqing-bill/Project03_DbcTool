#include "canwebsocketclient.h"
#include <QDebug>

CanWebSocketClient::CanWebSocketClient(QObject *parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_connected(false)
{
    connect(m_webSocket, &QWebSocket::connected,
            this, &CanWebSocketClient::onConnected);

    connect(m_webSocket, &QWebSocket::disconnected,
            this, &CanWebSocketClient::onDisconnected);

    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &CanWebSocketClient::onErrorOccurred);
}

CanWebSocketClient::~CanWebSocketClient()
{
    if (m_connected)
        m_webSocket->close();
}

void CanWebSocketClient::connectToServer(const QString &url)
{
    if (m_connected)
    {
        return;
    }
    m_serverAddress = url;
    m_webSocket->open(QUrl(url));
}

void CanWebSocketClient::disconnect()
{
    if (m_connected)
        m_webSocket->close();
}

void CanWebSocketClient::sendTextMessage(const QString &message)
{
    if (!m_connected)
    {
        return;
    }
    m_webSocket->sendTextMessage(message);
}

void CanWebSocketClient::onConnected()
{
    m_connected = true;
    emit connected();
}

void CanWebSocketClient::onDisconnected()
{
    m_connected = false;
    emit disconnected();
}

void CanWebSocketClient::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    QString errorStr = m_webSocket->errorString();
    emit errorOccurred(errorStr);
}

void CanWebSocketClient::sendBinaryMessage(const QByteArray &data)
{
    qDebug() << "是否重复打印";
    if (!m_connected)
        return;
    m_webSocket->sendBinaryMessage(data);
}
