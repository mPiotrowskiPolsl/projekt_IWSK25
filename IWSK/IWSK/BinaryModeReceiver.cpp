#include "BinaryModeReceiver.h"
#include "PortManager.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <conio.h>
#include <windows.h>

//OSOBA 10 DODAJ TU SWOJ KOD, CZÊŒÆ 6.2 i podstawy twojej czêœci GOTOWE


BinaryModeReceiver::BinaryModeReceiver() {}


void BinaryModeReceiver::receiveBinary() {

    HANDLE handle = PortManager::getHandle();

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[BinaryModeReceiver] Nieprawidlowy port COM" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(BUFFER_SIZE);
    DWORD bytesRead;

    std::cout << "\n Tryb binarny - odbieranie danych " << std::endl;
    std::cout << "Oczekiwanie na dane... (nacisnij Esc, aby zakonczyc)\n" << std::endl;

    while (true) {
        // po kliknieciu ESC powinno sie zatrzymac (nie dowiemy sie dopoki nie przetestujemy)
        if (_kbhit() && _getch() == 27) {
            std::cout << "\nZakonczono odbior danych binarnych." << std::endl;
            break;
        }

       
        BOOL result = ReadFile(handle, buffer.data(), BUFFER_SIZE, &bytesRead, nullptr);
        if (!result) {
            std::cerr << "[BinaryModeReceiver] Blad odczytu (kod: " << GetLastError() << ")" << std::endl;
            break;
        }

        if (bytesRead > 0) {
            
            std::cout << "[RX] Odebrano bajty: ";
            for (DWORD i = 0; i < bytesRead; ++i) {
                std::cout
                    << std::hex << std::uppercase
                    << std::setw(2) << std::setfill('0')
                    << static_cast<int>(buffer[i])
                    << ' ';
            }
            
            std::cout << std::dec << std::setfill(' ') << std::endl;
        }

        Sleep(10);
    }
}
