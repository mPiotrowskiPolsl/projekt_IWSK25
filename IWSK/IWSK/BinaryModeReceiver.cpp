#include "BinaryModeReceiver.h"
#include "PortManager.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <sstream>
#include <fstream>

//OSOBA 10 DODAJ TU SWOJ KOD, CZÊŒÆ 6.2 i podstawy twojej czêœci GOTOWE

extern std::wstring receivedText;
#define WM_UPDATE_TEXT (WM_USER + 1)


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

void BinaryModeReceiver::sendBinary() {
    HANDLE handle = PortManager::getHandle();

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Nieprawidlowy port COM" << std::endl;
        return;
    }

    std::cin.ignore();

    std::string inputLine;
    std::cout << "\nTryb binarny - nadawanie danych" << std::endl;
    std::cout << "Wprowadz bajty w formacie heksadecymalnym";
    std::getline(std::cin, inputLine);

    std::vector<uint8_t> buffer = parseHexInput(inputLine);

    char choice;
    std::cout << "Czy dopisac terminator (CR - 0D)? [t/n]";
    std::cin >> choice;
    if (choice == 't' || choice == 'T') {
        buffer.push_back(0x0D);
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(handle, buffer.data(), buffer.size(), &bytesWritten, nullptr);

    if (!result || bytesWritten != buffer.size()) {
        std::cerr << "Blad wysylania danych (kod: " << GetLastError() << ")" << std::endl;
    }
    else {
        std::cout << "[TX] Wyslano " << bytesWritten << " bajtow." << std::endl;
    }
}

std::vector<uint8_t> BinaryModeReceiver::parseHexInput(const std::string& input) {
    std::vector<uint8_t> output;
    std::istringstream stream(input);
    std::string hexByte;

    while (stream >> hexByte) {
        try {
            uint8_t byte = static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16));
            output.push_back(byte);
        }
        catch (...) {
            std::cerr << "Nieprawidlowy bajt: " << hexByte << std::endl;
        }
    }
    return output;
}

void BinaryModeReceiver::sendFile(const std::string& filePath) {
    HANDLE handle = PortManager::getHandle();

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "[sendFile] Nieprawidlowy port COM" << std::endl;
        return;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << filePath << std::endl;
        return;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    if (buffer.empty()) {
        std::cerr << "[sendFile] Plik jest pusty!" << std::endl;
        return;
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(handle, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesWritten, nullptr);

    if (!result || bytesWritten != buffer.size()) {
        std::cerr << "[sendFile] Blad wysylania (kod: " << GetLastError() << ")" << std::endl;
    }
    else {
        std::cout << "[TX] Wyslano " << bytesWritten << " bajtow z pliku." << std::endl;
    }


}

std::wstring BinaryModeReceiver::receiveBinaryToString() {
    HANDLE handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE) {
        return L"B³¹d: nieprawid³owy port COM.\n";
    }

    std::wstring resultText;
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    DWORD bytesRead;

    //while (true) {
        BOOL success = ReadFile(handle, buffer.data(), BUFFER_SIZE, &bytesRead, nullptr);
        if (!success) {
            resultText += L"\nB³¹d odczytu.\n";
            //break;
        }

        if (bytesRead > 0) {
            for (DWORD i = 0; i < bytesRead; ++i) {
                wchar_t byteHex[6];
                swprintf(byteHex, 6, L"%02X ", buffer[i]);
                resultText += byteHex;
            }
            resultText += L"\n";

            // Zaktualizuj dane w GUI
            
            //receivedText = resultText;
            
            //PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);
        }
        else {
            resultText = L"[ReceiveBinary]Brak danych";
        }
        return resultText;
        // Przerwanie w¹tku mo¿esz dodaæ przez jak¹œ flagê np. stopRequested
        //Sleep(100);
    //}

    return resultText;
}

