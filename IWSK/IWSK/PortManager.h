#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <windows.h>
#include <string>
#include <vector>
#include <stdexcept>

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
};

#endif