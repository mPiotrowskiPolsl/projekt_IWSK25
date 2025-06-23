#include "TextModeSender.h"
#include <windows.h>
#include <iostream>

TextModeSender::TextModeSender() {}

void TextModeSender::sendTextFromGUI(HANDLE handle, const std::string& message, const std::string& terminator) {
    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[TextModeSender] Nieprawidlowy uchwyt portu COM!" << std::endl;
        return;
    }

    std::string fullMessage = message + terminator;

    // Log do konsoli
    std::cout << "\n=== Tryb transmisji tekstowy - NADAWANIE (GUI) ===" << std::endl;
    std::cout << "[Bufor] Tekst do wyslania:\n" << message << std::endl;
    std::cout << "[Bufor] Terminator: ";
    if (terminator.empty()) {
        std::cout << "(brak)";
    }
    else {
        for (char c : terminator) {
            if (c == '\r') std::cout << "\\r";
            else if (c == '\n') std::cout << "\\n";
            else std::cout << c;
        }
    }
    std::cout << std::endl;

    DWORD bytesWritten;
    BOOL result = WriteFile(
        handle,
        fullMessage.c_str(),
        static_cast<DWORD>(fullMessage.size()),
        &bytesWritten,
        nullptr
    );

    if (!result) {
        std::cerr << "[TextModeSender] Blad podczas wysylania (kod: " << GetLastError() << ")" << std::endl;
    }
    else {
        std::cout << "[TX] Wyslano " << bytesWritten << " bajtow." << std::endl;
    }
}