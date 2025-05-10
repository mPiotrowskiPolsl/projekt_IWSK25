#include "TextModeReceiver.h"

TextModeReceiver::TextModeReceiver() {}

void TextModeReceiver::receiveText() {
    HANDLE handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[TextModeReceiver] Nieprawid³owy uchwyt portu COM!" << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    std::string receivedData;

    std::cout << "\n=== Tryb tekstowy - odbieranie danych ===" << std::endl;
    std::cout << "Oczekiwanie na dane... (naciœnij Esc, aby zakoñczyæ)" << std::endl;

    bool running = true;
    while (running) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27) { 
                std::cout << "Zakonczono odbieranie danych." << std::endl;
                break;
            }
        }


        BOOL result = ReadFile(handle, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!result) {
            std::cerr << "[TextModeReceiver] Blad odczytu (kod: " << GetLastError() << ")" << std::endl;
            break;
        }

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            receivedData.append(buffer, bytesRead);

            std::cout << "[RX] Odebrano tekst: " << buffer << std::endl;
        }

        Sleep(10);
    }

    if (!receivedData.empty()) {
        std::cout << "\n=== Odebrane dane tekstowe (calosc) ===" << std::endl;
        std::cout << receivedData << std::endl;
        std::cout << "=== Koniec odbioru ===" << std::endl;
    }
    else {
        std::cout << "[RX] Brak odebranych danych tekstowych." << std::endl;
    }
}