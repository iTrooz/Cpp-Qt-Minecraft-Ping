#include <QObject>
#include <QTcpSocket>

#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80

// Client for the Minecraft protocol
class McClient : public QObject {
    Q_OBJECT

    QString domain;
    QString ip;
    short port;
    QTcpSocket socket;
    QByteArray data;

public:
    explicit McClient(QObject *parent, QString domain, QString ip, short port): QObject(parent), domain(domain), ip(ip), port(port) {}

    void writeByte(char value) {
        data.append(value);
    }

    void writeVarInt(int value) {
        while (true) {
            if ((value & ~SEGMENT_BITS) == 0) {
                writeByte(value);
                return;
            }

            writeByte((value & SEGMENT_BITS) | CONTINUE_BIT);

            // Note: >>> means that the sign bit is shifted with the rest of the number rather than being left alone
            value >>= 7;
        }
    }

    void writeShort(short value) {
        data.append((char) (value >> 8));
        data.append((char) value);
    }

    void writeString(std::string value) {
        data.append(value);
    }

    void writeToSocket() {
        socket.write(data);
        socket.flush();
        data = QByteArray();
    }


    void getOnlinePlayers() {
        socket.connectToHost(ip, port);

        if (!socket.waitForConnected(3000)) {
            printf("Failed to connect to socket\n");
            return;
        }

        QByteArray data;
        writeByte(0x00); // packet ID
        writeVarInt(0x760); // protocol version
        writeByte(domain.size()); // server address length
        writeString(domain.toStdString()); // server address
        writeShort(port); // server port
        writeVarInt(1); // next state

        writeToSocket();
    }

};