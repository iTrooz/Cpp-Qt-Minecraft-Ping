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

public:
    explicit McClient(QObject *parent, QString domain, QString ip, short port): QObject(parent), domain(domain), ip(ip), port(port) {}

    void getOnlinePlayers() {
        socket.connectToHost(ip, port);

        if (!socket.waitForConnected(3000)) {
            printf("Failed to connect to socket\n");
            return;
        }

        QByteArray data;
        writeVarInt(data, 0x00); // packet ID
        writeVarInt(data, 0x760); // protocol version
        writeVarInt(data, domain.size()); // server address length
        writeString(data, domain.toStdString()); // server address
        writeFixedInt(data, port, 2); // server port
        writeVarInt(data, 0x01); // next state
        writePacketToSocket(data); // send handshake packet

        data.clear();

        writeVarInt(data, 0x00); // packet ID
        writePacketToSocket(data); // send status packet

        if (!socket.waitForReadyRead(3000)) {
            printf("Socket didn't send anything to read\n");
            return;
        }

        auto resp = socket.readAll();
        printf("RESP SIZE=%d\n", resp.size());
    }

private:
    // From https://wiki.vg/Server_List_Ping
    void writeVarInt(QByteArray &data, int value) {
        while (true) {
            if ((value & ~SEGMENT_BITS) == 0) {
                data.append(value);
                return;
            }

            data.append((value & SEGMENT_BITS) | CONTINUE_BIT);

            // Note: >>> means that the sign bit is shifted with the rest of the number rather than being left alone
            value >>= 7;
        }
    }

    // write number with specified size in big endian format
    void writeFixedInt(QByteArray &data, int value, int size) {
        for (int i = size - 1; i >= 0; i--) {
            data.append((value >> (i * 8)) & 0xFF);
        }
    }

    void writeString(QByteArray &data, std::string value) {
        data.append(value);
    }

    void writePacketToSocket(QByteArray &data) {
        // we prefix the packet with its length
        QByteArray dataWithSize;
        writeVarInt(dataWithSize, data.size());
        dataWithSize.append(data);

        // write it to the socket
        socket.write(data);
        socket.flush();
    }
};
