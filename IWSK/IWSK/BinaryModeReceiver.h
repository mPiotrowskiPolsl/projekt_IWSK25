#pragma once

#include <cstdint>
#include <vector>
#include <string>
// OSOBA 10 DODAJ TU SWOJ KOD CZʌ� 6.2 i podstawy twojej cz�ci GOTOWE
class BinaryModeReceiver {

public:

    BinaryModeReceiver();


    void receiveBinary();
    void sendBinary();
    void sendFile(const std::string& filePath);

private:
    std::vector<uint8_t> parseHexInput(const std::string& input);

    static const int BUFFER_SIZE = 256;

};
