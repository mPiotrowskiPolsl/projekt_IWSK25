#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <windows.h>
// OSOBA 10 DODAJ TU SWOJ KOD CZÊŒÆ 6.2 i podstawy twojej czêœci GOTOWE
class BinaryModeReceiver {

public:

    BinaryModeReceiver();


    void receiveBinary();
    void sendBinary();
    void sendFile(const std::string& filePath);
    std::wstring receiveBinaryToString(HWND hwnd);

    std::vector<uint8_t> parseHexInput(const std::string& input);

    static const int BUFFER_SIZE = 256;

};
