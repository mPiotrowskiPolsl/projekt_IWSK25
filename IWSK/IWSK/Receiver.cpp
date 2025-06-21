#include "Receiver.h"
#include "PortManager.h"
#include <iostream>

#define BUFFER_SIZE 256

extern PortManager portmanager;  // zakładamy, że obiekt istnieje globalnie


#define WM_UPDATE_TEXT (WM_USER + 1)  // własny identyfikator komunikatu


Receiver::Receiver() {}

std::string Receiver::receive() {
    HANDLE handle = portmanager.getHandle();  // poprawne użycie instancji
    std::string output;


    //return "test";

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[Receiver] Nieprawidłowy uchwyt portu COM!" << std::endl;
        return "[Błąd] Nieprawidłowy uchwyt!";
    }

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    std::cout << "\n=== Odbieranie danych (1x) ===" << std::endl;

    BOOL result = ReadFile(handle, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
    if (!result) {
        std::cerr << "[Receiver] Błąd odczytu (kod: " << GetLastError() << ")" << std::endl;
        return "[Błąd] ReadFile nieudany!";
    }

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';  // zakończenie ciągu tekstowego
        output = buffer;
        std::cout << "[RX] Odebrano: " << output << std::endl;
    }
    else {
        output = "[RX] Brak danych.";
        std::cout << output << std::endl;
    }

    return output;
}
