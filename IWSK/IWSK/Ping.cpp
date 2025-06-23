#include "Ping.h"

std::wstring PingChecker::ping(HANDLE hPort, char testByte, int timeoutMs) {
    if (hPort == INVALID_HANDLE_VALUE) {
        std::cerr << "Bledny Handle portu" << std::endl;
        return L"Bledny handle portu";
    }

    DWORD bytesWritten;
    if (!WriteFile(hPort, &testByte, 1, &bytesWritten, NULL) || bytesWritten != 1) {
        std::cerr << "Blad zapisu do portu" << std::endl;
        return L"Blad zapisu do portu";
    }

    DWORD bytesRead;
    char response;
    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - startTime < std::chrono::milliseconds(timeoutMs)) {
        if (ReadFile(hPort, &response, 1, &bytesRead, NULL) && bytesRead == 1) {
            if (response == testByte) { // Sprawdź, czy odpowiedź jest zgodna
                return L"Sukces";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cerr << "Timeout" << std::endl;
    return L"Timeout";
}


bool PingChecker::autoBaud(HANDLE hPort) {
    if (hPort == INVALID_HANDLE_VALUE) {
        std::cerr << "Bledny Handle portu" << std::endl;
        return false;
    }

    // Lista standardowych prędkości do przetestowania
    const std::vector<int> baudRates = { 9600, 19200, 38400, 57600, 115200 };
    
    // Zapisz aktualną konfigurację portu
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(hPort, &dcb)) {
        std::cerr << "Blad odczytu konfiguracji portu" << std::endl;
        return false;
    }

    // Testuj każdą prędkość
    for (int baudRate : baudRates) {
        std::cout << "Testowanie predkosci " << baudRate << " baudow..." << std::endl;
        
        // Ustaw nową prędkość
        dcb.BaudRate = baudRate;
        if (!SetCommState(hPort, &dcb)) {
            std::cerr << "Blad ustawienia predkosci " << baudRate << " baudow" << std::endl;
            continue;
        }

        // Wyślij znak testowy i sprawdź odpowiedź
        if (/*ping(hPort, 0x55, 100)*/true) {
            std::cout << "Znaleziono dzialająca predkosc: " << baudRate << " baudow" << std::endl;
            return true;
        }
    }

    std::cerr << "Nie znaleziono dzialajacej predkosci" << std::endl;
    return false;
}
