#include<stdio.h>
#include<string>
#include<iostream>

#include <QObject>
#include <QtNetwork/QDnsLookup>
#include <QCoreApplication>

#include <minecraft_ping.hpp>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc != 3) {
        printf ("Usage: %s <domain> <port>\n", argv[0]);
        return 1;
    }
    std::string domain = argv[1];
    int port = std::stoi(argv[2]);

    MinecraftPing ping(nullptr, domain, port);
    QObject::connect(&ping, &MinecraftPing::succeed, [&](){
        std::cout << "OK !" << std::endl;
    });
    QObject::connect(&ping, &MinecraftPing::fail, [&](){
        std::cout << "Fail !" << std::endl;
    });
    ping.ping();

    return app.exec();
}
