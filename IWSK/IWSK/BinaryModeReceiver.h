#pragma once

#include <cstdint>
// OSOBA 10 DODAJ TU SWOJ KOD CZʌ� 6.2 i podstawy twojej cz�ci GOTOWE
class BinaryModeReceiver {

public:

    BinaryModeReceiver();


    void receiveBinary();

private:

    static const int BUFFER_SIZE = 256;

};
