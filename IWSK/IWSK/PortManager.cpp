#include "PortManager.h"
#include <iostream>
#include <winreg.h>
#include <locale>
#include <windows.h>

// Definicje statycznych zmiennych
HANDLE PortManager::hPort = INVALID_HANDLE_VALUE;
std::string PortManager::selectedPort = "";
int PortManager::baudRate = 9600;
int PortManager::dataBits = 8;
char PortManager::parity = 'N';
int PortManager::stopBits = 1;

// Konwersja std::string na std::wstring
std::wstring stringToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
    return wstr;
}

// Konwersja std::wstring na std::string
std::string wideToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, nullptr, nullptr);
    return str;
}

std::vector<std::string> PortManager::getAvailablePorts() {
    std::vector<std::string> ports;
    HKEY hKey;
    LPCWSTR subKey = L"HARDWARE\\DEVICEMAP\\SERIALCOMM";

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        throw std::runtime_error("Brak dostepu do rejestru systemowego");
    }

    wchar_t valueName[256];
    BYTE data[256];
    DWORD valueNameSize, dataSize, type, index = 0;

    while (true) {
        valueNameSize = sizeof(valueName) / sizeof(wchar_t);
        dataSize = sizeof(data);
        if (RegEnumValueW(hKey, index++, valueName, &valueNameSize, nullptr, &type, data, &dataSize) != ERROR_SUCCESS) break;

        if (type == REG_SZ) {
            std::wstring wdata(reinterpret_cast<wchar_t*>(data));
            ports.push_back(wideToString(wdata));
        }
    }

    RegCloseKey(hKey);
    return ports;
}

bool PortManager::selectPort() {
    try {
        auto ports = getAvailablePorts();
        if (ports.empty()) {
            std::cerr << "Brak wykrytych portow COM!" << std::endl;
            return false;
        }

        std::cout << "\n=== Dostepne porty COM ===" << std::endl;
        for (size_t i = 0; i < ports.size(); ++i) {
            std::cout << i + 1 << ". " << ports[i] << std::endl;
        }

        int choice;
        std::cout << "Wybierz numer portu (1-" << ports.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || choice > static_cast<int>(ports.size())) {
            std::cerr << "Nieprawidlowy wybor!" << std::endl;
            return false;
        }

        selectedPort = ports[choice - 1];
        std::wstring widePort = L"\\\\.\\" + stringToWide(selectedPort);
        hPort = CreateFileW(widePort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hPort == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Nie mozna otworzyc portu (kod bledu: " + std::to_string(GetLastError()));
        }

        configurePort();
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "[BLAD] " << e.what() << std::endl;
        return false;
    }
}

void PortManager::configureSpeed() {
    const std::vector<int> speeds = { 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
    std::cout << "\n=== Konfiguracja predkosci transmisji ===" << std::endl;
    for (size_t i = 0; i < speeds.size(); ++i) {
        std::cout << i + 1 << ". " << speeds[i] << " baudow" << std::endl;
    }

    int choice;
    std::cout << "Wybierz predkosc (1-" << speeds.size() << "): ";
    std::cin >> choice;

    if (choice < 1 || choice > static_cast<int>(speeds.size())) {
        baudRate = 9600;
        std::cerr << "Nieprawidlowy wybor. Ustawiono domyslna predkosc 9600 baudow\n";
        return;
    }

    baudRate = speeds[choice - 1];
    std::cout << "Ustawiono predkosc: " << baudRate << " baudow\n" << std::endl;
}

void PortManager::configureFrame() {
    std::cout << "\n=== Konfiguracja ramki ===" << std::endl;

    while (true) {
        std::cout << "Podaj liczbe bitow danych (7/8): ";
        std::cin >> dataBits;
        if (dataBits == 7 || dataBits == 8) break;
        std::cerr << "Nieprawidlowa wartosc! ";
    }

    while (true) {
        std::cout << "Podaj kontrole parzystosci (E - even, O - odd, N - none): ";
        char p;
        std::cin >> p;
        p = toupper(p);
        if (p == 'E' || p == 'O' || p == 'N') {
            parity = p;
            break;
        }
        std::cerr << "Nieprawidlowy wybor! ";
    }

    while (true) {
        std::cout << "Podaj liczbe bitow stopu (1/2): ";
        std::cin >> stopBits;
        if (stopBits == 1 || stopBits == 2) break;
        std::cerr << "Nieprawidlowa wartosc! ";
    }

    if (hPort != INVALID_HANDLE_VALUE) configurePort();
    std::cout << "\nKonfiguracja ramki: "
        << dataBits << " bitow danych, "
        << "parzystosc " << parity << ", "
        << stopBits << " bity stopu\n" << std::endl;
}

void PortManager::configurePort() {
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(hPort, &dcb)) {
        throw std::runtime_error("Blad odczytu konfiguracji portu");
    }

    dcb.BaudRate = baudRate;
    dcb.ByteSize = dataBits;
    dcb.Parity = (parity == 'E') ? EVENPARITY : (parity == 'O') ? ODDPARITY : NOPARITY;
    dcb.StopBits = (stopBits == 1) ? ONESTOPBIT : TWOSTOPBITS;

    if (!SetCommState(hPort, &dcb)) {
        throw std::runtime_error("Blad konfiguracji portu (kod: " + std::to_string(GetLastError()) + ")");
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(hPort, &timeouts);
}

void PortManager::closePort() {
    if (hPort != INVALID_HANDLE_VALUE) {
        CloseHandle(hPort);
        hPort = INVALID_HANDLE_VALUE;
    }
}