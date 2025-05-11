#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>

class BinaryModeSender
{
public:
    BinaryModeSender();
    void readHexBytesFromConsole();
    void sendWithTerminator(uint8_t terminator = 0x0D);
private:
    HANDLE handle;
    std::vector<uint8_t> txBuffer;
};