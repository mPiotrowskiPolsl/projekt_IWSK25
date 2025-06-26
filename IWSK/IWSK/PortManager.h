#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <windows.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

class PortManager {
private:
    static HANDLE hPort;
    static std::string selectedPort;
    static int baudRate;
    static int dataBits;
    static char parity;
    static int stopBits;

    static void configurePort();

public:
    static std::vector<std::string> getAvailablePorts();
    static bool selectPort(int sel);
    static void configureSpeed();
    static void configureFrame();
    static void closePort();

    static HANDLE getHandle() { return hPort; }
    static std::string getPort() { return selectedPort; }
    static int getBaudRate() { return baudRate; }
    static int getDataBits() { return dataBits; }
    static char getParity() { return parity; }
    static int getStopBits() { return stopBits; }

    static void setBaudRate(int baud) { baudRate = baud; configurePort(); std::cerr <<"ustawiono baudrate: " << baud << std::endl; }
    static void setDataBits(int bits) { dataBits = bits; configurePort(); }
    static void setParity(char p) { parity = p; configurePort(); }
    static void setStopBits(int bits) { stopBits = bits; configurePort(); }
};

#endif