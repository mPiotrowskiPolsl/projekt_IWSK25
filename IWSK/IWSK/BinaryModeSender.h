#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>
#include <string>

class BinaryModeSender
{
public:
    BinaryModeSender();
    void readHexBytesFromConsole();
    void sendWithTerminator(uint8_t terminator = 0x0D);
    void sendFile(const std::string& filename, uint8_t terminator);
private:
    HANDLE handle;
    std::vector<uint8_t> txBuffer;
};