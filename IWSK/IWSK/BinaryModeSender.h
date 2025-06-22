#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>
#include <string>
#include "Terminator.h"

class BinaryModeSender
{
public:
    BinaryModeSender();
    void readHexBytesFromConsole();
    void sendWithTerminator(uint8_t terminator = 0x0D);
    void sendFile(const std::string& filename, uint8_t terminator);
    void sendFromHex(const WCHAR* hexInput, const Terminator& terminator);
private:
    HANDLE handle;
    std::vector<uint8_t> txBuffer;
};