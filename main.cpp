#include<stdio.h>
#include<string>
#include<iostream>

#include <QObject>
#include <QtNetwork/QDnsLookup>
#include <QCoreApplication>

#include <minecraft_ping.hpp>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    MinecraftPing ping(nullptr, "hypixel.net", 25565);
    QObject::connect(&ping, &MinecraftPing::succeed, [&](){
        std::cout << "OK !" << std::endl;
    });
    QObject::connect(&ping, &MinecraftPing::fail, [&](){
        std::cout << "Fail !" << std::endl;
    });
    ping.ping();

    return app.exec();
}
