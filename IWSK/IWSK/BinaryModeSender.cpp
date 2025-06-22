#include "BinaryModeSender.h"
#include "PortManager.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cwchar>
#include <locale>
#include <codecvt>
#include <vector>

BinaryModeSender::BinaryModeSender(HWND hwnd)
{
    handle = PortManager::getHandle();
    if (handle == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, L"Nie udalo sie uzyskac uchwytu portu COM.", L"Info", MB_OK);
    }
}

void BinaryModeSender::sendFromHex(HWND hwnd, const WCHAR* hexInput, const Terminator& terminator)
{
    if (!hexInput)
    {
        MessageBox(hwnd, L"Nie podano wartosci binarnych.", L"Info", MB_OK);
        return;
    }

    std::wstring wstr(hexInput);

    if (wstr.length() % 2 != 0)
    {
        MessageBox(hwnd, L"Podaj parzysta liczbe znakow.", L"Info", MB_OK);
        return;
    }

    txBuffer.clear();

    for (size_t i = 0; i < wstr.length(); i += 2)
    {
        std::wstring hexByteStr = wstr.substr(i, 2);
        try
        {
            uint8_t byte = static_cast<uint8_t>(std::stoul(hexByteStr, nullptr, 16));
            txBuffer.push_back(byte);
        }
        catch (...) {
            std::wcerr << L"Nieprawid³owy bajt: " << hexByteStr << std::endl;
        }
    }

    std::string term = terminator.get();
    txBuffer.insert(txBuffer.end(), term.begin(), term.end());

    if (handle == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, L"Nieprawidlowy uchwyt portu COM.", L"Info", MB_OK);
        return;
    }

    DWORD bytesWritten;
    if (!WriteFile(handle, txBuffer.data(), static_cast<DWORD>(txBuffer.size()), &bytesWritten, nullptr))
    {
        MessageBox(hwnd, L"Blad przy wysylaniu danych.", L"Info", MB_OK);
    }
    else
    {
        std::wstring helper = L"Wyslano ";
        helper += std::to_wstring(bytesWritten) + L" bajtow";
        MessageBox(hwnd, helper.c_str(), L"Info", MB_OK);
    }
}

bool BinaryModeSender::readAndSendFile(HWND hwnd, const wchar_t* filePath, const Terminator& terminator)
{
    std::ifstream plo(filePath, std::ios::binary);

    if (!plo)
    {
        MessageBox(hwnd, L"Blad podczas otwarcia pliku", L"Info", MB_OK);
    }

    uint8_t character;

    
    txBuffer.clear();

    while (plo.read((char*)&character, sizeof(uint8_t)))
    {
        txBuffer.push_back(character);
    }

    std::string term = terminator.get();
    txBuffer.insert(txBuffer.end(), term.begin(), term.end());

    if (handle == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"Nieprawidlowy uchwyt portu", L"Info", MB_OK);
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(handle, txBuffer.data(), static_cast<DWORD>(txBuffer.size()), &bytesWritten, nullptr)) {
        MessageBox(hwnd, L"Blad przy wysylaniu danych", L"Info", MB_OK);
        return false;
    }
    else {
        std::wstring helper = L"Wyslano ";
        helper += std::to_wstring(bytesWritten) + L" bajtow";
        MessageBox(hwnd, helper.c_str(), L"Info", MB_OK);

        return true;
    }
}