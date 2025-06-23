#ifndef PING_H
#define PING_H

#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

class PingChecker {
public:
    static std::wstring ping(HANDLE hPort, char testByte = 0x55, int timeoutMs = 1000);
    static bool autoBaud(HANDLE hPort);
};

#endif // PING_H