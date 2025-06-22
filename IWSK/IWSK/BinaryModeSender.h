#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>
#include <string>
#include "Terminator.h"
#include <fstream>

class BinaryModeSender
{
public:
    BinaryModeSender(HWND hwnd);
    void sendFromHex(HWND hwnd, const WCHAR* hexInput, const Terminator& terminator);
    bool readAndSendFile(HWND hwnd, const wchar_t* filePath, const Terminator& terminator);
private:
    HANDLE handle;
    std::vector<uint8_t> txBuffer;
};