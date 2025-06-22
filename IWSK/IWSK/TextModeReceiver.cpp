#include "TextModeReceiver.h"
#include "PortManager.h"
#include <iostream>
#include <conio.h>  // dla _kbhit() i _getch()

extern std::wstring receivedText;
#define WM_UPDATE_TEXT (WM_USER + 1)

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

std::wstring TextModeReceiver::receiveTextToString(HWND hwnd) {
    HANDLE handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE) {
        return L"Błąd: nieprawidłowy port COM.\n";
    }

    std::wstring resultText;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(handle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
        if (!success) {
            resultText += L"\nBłąd odczytu.\n";
            break;
        }

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            std::string receivedStr(buffer, bytesRead);
            std::wstring wideStr(receivedStr.begin(), receivedStr.end());

            resultText += wideStr;
            resultText += L"\n";

            receivedText = resultText;
            PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);
        }

        Sleep(100);
    }

    return resultText;
}