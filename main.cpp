#include<stdio.h>
#include<string>
#include<iostream>

#include <QObject>
#include <QtNetwork/QDnsLookup>
#include <QCoreApplication>

#include <mc_resolver.hpp>
#include <mc_client.hpp>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc < 2) {
        printf ("Usage: %s <domain> [port]\n", argv[0]);
        return 1;
    }
    std::string domain = argv[1];


    int port = 25565;
    if (argc >= 3) {
        port = std::stoi(argv[2]);
    }

    MCResolver resolver(nullptr, domain, port);
    QObject::connect(&resolver, &MCResolver::succeed, [&](QString ip, int port) {
        qDebug() << "Resolved Addresse for" << domain << ": " << ip << ":" << port;
        McClient client(nullptr, QString::fromStdString(domain), ip, port);
        int online = client.getOnlinePlayers();
        printf("Online players: %d\n", online);

        client.close();
        app.quit();
    });
    resolver.ping();

    return app.exec();
}
