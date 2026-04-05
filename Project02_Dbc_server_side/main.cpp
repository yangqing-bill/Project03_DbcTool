#include <QCoreApplication>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QProtobufSerializer>
#include "CanMessage/canframe.qpb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QWebSocketServer server("DbcServer", QWebSocketServer::NonSecureMode);
    if (!server.listen(QHostAddress::LocalHost, 8080)) {
        qDebug() << "Failed to listen on port 8080";
        return -1;
    }
    qDebug() << "WebSocket server started at ws://localhost:8080 (supports JSON and Protobuf)";

    QObject::connect(&server, &QWebSocketServer::newConnection, [&]() {
        QWebSocket *socket = server.nextPendingConnection();
        qDebug() << "Client connected from" << socket->peerAddress().toString()<< socket;

        // 处理文本消息（JSON）
        QObject::connect(socket, &QWebSocket::textMessageReceived, [socket](const QString &message) {
            QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
            if (doc.isNull()) {
                qDebug() << "Invalid JSON received";
                return;
            }
            QJsonObject obj = doc.object();
            QString messageIdHex = "0x" + QString::number(obj["message_id"].toInt(), 16).toUpper();

            qDebug() << "[" << QDateTime::currentDateTime().toString() << "]"
                     << "[ID:" << messageIdHex << "]"
                     << "[Name:" << obj["message_name"].toString() << "] (JSON)";

            QJsonArray sigArray = obj["signals"].toArray();
            for (const QJsonValue &val : sigArray) {
                QJsonObject sig = val.toObject();
                qDebug() << "  -" << sig["name"].toString() << ":"
                         << sig["physical_value"].toDouble() << sig["unit"].toString()
                         << "(raw:" << sig["raw_value"].toDouble() << ")";
            }
            qDebug() << "";
        });

        // 处理二进制消息（Protobuf）
        QObject::connect(socket, &QWebSocket::binaryMessageReceived, [socket](const QByteArray &message) {
            QProtobufSerializer serializer;
            CanMessage::CanFrame frame;
            if (!serializer.deserialize(&frame, message)) {
                qDebug() << "Failed to deserialize Protobuf message";
                return;
            }

            QString messageIdHex = "0x" + QString::number(frame.messageId(), 16).toUpper();
            qDebug() << "[" << QDateTime::currentDateTime().toString() << "]"
                     << "[ID:" << messageIdHex << "]"
                     << "[Name:" << frame.messageName() << "] (Protobuf)";

            const QList<CanMessage::Signal> &sigList  = frame.signals_();
            for (const CanMessage::Signal &sig : sigList ) {
                qDebug() << "  -" << sig.name() << ":"
                         << sig.physicalValue() << sig.unit()
                         << "(raw:" << sig.rawValue() << ")";
            }
            qDebug() << "";
        });

        QObject::connect(socket, &QWebSocket::disconnected, [socket]() {
            qDebug() << "Client disconnected";
            socket->deleteLater();
        });
    });

    return a.exec();
}
