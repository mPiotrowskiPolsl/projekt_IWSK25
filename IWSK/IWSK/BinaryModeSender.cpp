#include "BinaryModeSender.h"
#include "PortManager.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cwchar>
#include <locale>
#include <codecvt>

BinaryModeSender::BinaryModeSender()
{
    handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "B³¹d: Nie uda³o siê uzyskaæ uchwytu portu COM." << std::endl;
    }
}

void BinaryModeSender::readHexBytesFromConsole()
{
    std::string line;
    std::cout << "WprowadŸ bajty w formacie hex: ";
    std::getline(std::cin, line);

    std::istringstream iss(line);
    std::string byteStr;
    txBuffer.clear();

    while (iss >> byteStr)
    {
        try
        {
            uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
            txBuffer.push_back(byte);
        }
        catch (...) {
            std::cerr << "Nieprawid³owy bajt: " << byteStr << std::endl;
        }
    }

    std::cout << "Bufor do wys³ania: ";
    for (auto b : txBuffer)
    {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << std::endl;
}

void BinaryModeSender::sendWithTerminator(uint8_t terminator)
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Nieprawid³owy uchwyt portu!" << std::endl;
        return;
    }

    txBuffer.push_back(terminator);

    DWORD bytesWritten;
    if (!WriteFile(handle, txBuffer.data(), static_cast<DWORD>(txBuffer.size()), &bytesWritten, nullptr))
    {
        std::cerr << "B³¹d przy wysy³aniu danych!" << std::endl;
    }
    else
    {
        std::cout << "Wys³ano " << bytesWritten << " bajtów." << std::endl;
    }
}

void BinaryModeSender::sendFile(const std::string& filename, uint8_t terminator) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Nie mo¿na otworzyæ pliku: " << filename << std::endl;
        return;
    }

    txBuffer.clear();
    txBuffer.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    std::cout << "Wczytano " << txBuffer.size() << " bajtów z pliku." << std::endl;

    sendWithTerminator(terminator);
}

void BinaryModeSender::sendFromHex(const WCHAR* hexInput, const Terminator& terminator)
{
    if (!hexInput) {
        std::cerr << "Pusta tablica wejœciowa." << std::endl;
        return;
    }

    std::wstring wstr(hexInput);

    if (wstr.length() % 2 != 0) {
        std::cerr << "Nieparzysta liczba znaków – nie mo¿na utworzyæ bajtów z hex." << std::endl;
        return;
    }

    txBuffer.clear();

    for (size_t i = 0; i < wstr.length(); i += 2) {
        std::wstring hexByteStr = wstr.substr(i, 2);
        try {
            uint8_t byte = static_cast<uint8_t>(std::stoul(hexByteStr, nullptr, 16));
            txBuffer.push_back(byte);
        }
        catch (...) {
            std::wcerr << L"Nieprawid³owy bajt: " << hexByteStr << std::endl;
        }
    }

    std::string term = terminator.get();
    txBuffer.insert(txBuffer.end(), term.begin(), term.end());

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Nieprawid³owy uchwyt portu." << std::endl;
        return;
    }

    DWORD bytesWritten;
    if (!WriteFile(handle, txBuffer.data(), static_cast<DWORD>(txBuffer.size()), &bytesWritten, nullptr)) {
        std::cerr << "B³¹d przy wysy³aniu danych." << std::endl;
    }
    else {
        std::cout << "Wys³ano " << bytesWritten << " bajtów." << std::endl;
    }
}
