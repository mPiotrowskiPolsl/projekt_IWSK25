#include "BinaryModeSender.h"
#include "PortManager.h"

#include <iostream>
#include <sstream>
#include <iomanip>

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