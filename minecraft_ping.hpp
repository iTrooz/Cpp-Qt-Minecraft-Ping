#include <QObject>
#include <QDnsLookup>
#include <QtNetwork/qtcpsocket.h>
#include <QCoreApplication>
#include <QApplication>
#include <QHostInfo>

class MinecraftPing : public QObject {
    Q_OBJECT

    std::string domain;
    int port;

public:
    explicit MinecraftPing(QObject *parent): QObject(parent) {}
    explicit MinecraftPing(QObject *parent, std::string domain, int port): QObject(parent), domain(domain), port(port) {}
    virtual ~MinecraftPing() {};

    void ping() {
        pingWithDomainSRV();
    }

private:

    void pingWithDomainSRV() {
        QDnsLookup *lookup = new QDnsLookup(this);
        lookup->setName(QString("_minecraft._tcp.%1").arg(QString::fromStdString(domain)));
        lookup->setType(QDnsLookup::SRV);

        connect(lookup, &QDnsLookup::finished, this, [&]() {
            QDnsLookup *lookup = qobject_cast<QDnsLookup *>(sender());

            lookup->deleteLater();

            if (lookup->error() != QDnsLookup::NoError) {
                printf("Warning: SRV record lookup failed (%v), trying A record lookup\n", lookup->errorString().toStdString());
                emitFail(lookup->errorString().toStdString());
                return;
            }

            auto records = lookup->serviceRecords();
            if (records.isEmpty()) {
                printf("Warning: no SRV entries found for domain, trying A record lookup\n");
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

    void pingWithDomainA() {
        QHostInfo::lookupHost(QString::fromStdString(domain), this, [&](const QHostInfo &hostInfo){
            if (hostInfo.error() != QHostInfo::NoError) {
                pingWithDomainSRV();
                emitFail("A record lookup failed");
                return;
            } else {
                qDebug() << "Resolved Addresses for" << hostInfo.hostName() << ":";
                auto records = hostInfo.addresses();
                if (records.isEmpty()) {
                    emitFail("No A entries found for domain");
                    return;
                }
                
                
                const auto& firstRecord = records.at(0);
                pingWithIP(firstRecord.toString(), this->port);
            }
        });        
    }

    void pingWithIP(QString ip, int port) {
        printf("Found IP %s, port %d\n", ip.toStdString().c_str(), port);

        // TODO
        
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