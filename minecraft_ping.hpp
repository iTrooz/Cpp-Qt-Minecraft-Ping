#include <QObject>
#include <QDnsLookup>
#include <QtNetwork/qtcpsocket.h>
#include <QCoreApplication>
#include <QApplication>

class MinecraftPing : public QObject {
    Q_OBJECT

    std::string domain;
    int port;

public:
    explicit MinecraftPing(QObject *parent): QObject(parent) {}
    explicit MinecraftPing(QObject *parent, std::string domain, int port): QObject(parent), domain(domain), port(port) {}
    virtual ~MinecraftPing() {};

    void ping() {
        pingWithDomainA();
    }

private:

    void pingWithDomainA() {
        QDnsLookup *lookup = new QDnsLookup(this);
        lookup->setName(QString::fromStdString(domain));
        lookup->setType(QDnsLookup::A);

        connect(lookup, &QDnsLookup::finished, this, [&]() {
            QDnsLookup *lookup = qobject_cast<QDnsLookup *>(sender());

            lookup->deleteLater();

            if (lookup->error() != QDnsLookup::NoError) {
                printf("Warning: A record lookup failed (%v), trying SRV record lookup\n", lookup->errorString().toStdString());
                pingWithDomainSRV();
                return;
            }

            auto records = lookup->hostAddressRecords();
            if (records.isEmpty()) {
                printf("Warning: no A entries found for domain, trying SRV record lookup\n");
                pingWithDomainSRV();
                return;
            }


            const auto& firstRecord = records.at(0);
            pingWithIP(firstRecord.value().toString(), this->port);
        });

        lookup->lookup();
    }

    void pingWithDomainSRV() {
        QDnsLookup *lookup = new QDnsLookup(this);
        lookup->setName(QString::fromStdString(domain));
        lookup->setType(QDnsLookup::SRV);

        connect(lookup, &QDnsLookup::finished, this, [&]() {
            QDnsLookup *lookup = qobject_cast<QDnsLookup *>(sender());

            lookup->deleteLater();

            if (lookup->error() != QDnsLookup::NoError) {
                emitFail(lookup->errorString().toStdString());
                return;
            }

            auto records = lookup->serviceRecords();
            if (records.isEmpty()) {
                emitFail("No SRV entries found for domain");
                return;
            }


            const auto& firstRecord = records.at(0);
            QString ip = firstRecord.target();
            int port = firstRecord.port();
            pingWithIP(ip, port);
        });

        lookup->lookup();
    }

    void pingWithIP(QString ip, int port) {
        printf("Found IP %s, port %d\n", ip.toStdString().c_str(), port);

        QTcpSocket socket;
        socket.connectToHost(ip, port);
        if (!socket.waitForConnected(5000)) {
            emitFail("");
            return;
        }

        socket.write("ping");
        if (!socket.waitForReadyRead(5000)) {
            emitFail("");
            return;
        }

        auto response = socket.readAll();
        
        emitSucceed();
    }

    void emitFail(std::string error) {
        printf("Ping error: %s\n", error.c_str());
        emit fail();
    }

    void emitSucceed() {
        emit succeed();
    }

signals:
    void succeed();
    void fail();
};