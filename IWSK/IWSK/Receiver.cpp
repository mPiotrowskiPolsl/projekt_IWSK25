#include "Receiver.h"
#include "PortManager.h"  // aby mieæ dostêp do PortManager::hPort
#include <iostream>

#define BUFFER_SIZE 256

Receiver::Receiver() {
    // Mo¿na dodaæ logikê inicjalizacji jeœli potrzeba
}

void Receiver::receive() {
    HANDLE handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[Receiver] Nieprawidlowy uchwyt portu COM!" << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    std::cout << "\n=== Odbieranie danych (1x) ===" << std::endl;

    BOOL result = ReadFile(handle, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
    if (!result) {
        std::cerr << "[Receiver] Blad odczytu (kod: " << GetLastError() << ")" << std::endl;
        return;
    }

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // koñczymy ci¹g tekstowy
        std::cout << "[RX] Odebrano: " << buffer << std::endl;
    }
    else {
        std::cout << "[RX] Brak danych." << std::endl;
    }
}
