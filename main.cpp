#include<stdio.h>
#include<string>
#include<iostream>

#include <QObject>
#include <QtNetwork/QDnsLookup>
#include <QApplication>

#include <minecraft_ping.hpp>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MinecraftPing ping(nullptr, "example.com", 25565);
    QObject::connect(&ping, &MinecraftPing::succeed, [&](){
        std::cout << "OK !" << std::endl;
    });
    QObject::connect(&ping, &MinecraftPing::fail, [&](){
        std::cout << "Fail !" << std::endl;
    });
    ping.ping();

    return app.exec();
}
