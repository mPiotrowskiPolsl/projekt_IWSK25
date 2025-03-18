#include "Sender.h"
#include <iostream>

Sender::Sender() : hSerial(INVALID_HANDLE_VALUE), isSending(false) {
}

void Sender::initialize(HANDLE serialHandle) {
    std::lock_guard<std::mutex> lock(serialMutex);
    hSerial = serialHandle;
}

bool Sender::send(const std::string& data, const std::string& terminator) {
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "B³¹d: Port szeregowy nie jest otwarty." << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> lock(serialMutex);
    isSending = true;

    // Prepare data with terminator
    std::string dataToSend = data + terminator;
    DWORD bytesWritten = 0;

    // Write data to serial port
    bool success = WriteFile(hSerial, dataToSend.c_str(), dataToSend.length(), &bytesWritten, NULL);

    if (!success) {
        std::cerr << "B³¹d podczas wysy³ania danych: " << GetLastError() << std::endl;
        isSending = false;
        return false;
    }

    // Ensure all data was sent
    bool result = (bytesWritten == dataToSend.length());

    if (result) {
        std::cout << "Wys³ano dane: " << data << std::endl;
    }
    else {
        std::cerr << "Niepe³ne wys³anie danych: " << bytesWritten << " z " << dataToSend.length() << " bajtów." << std::endl;
    }

    isSending = false;
    return result;
}

bool Sender::sendBinary(const std::vector<unsigned char>& data, const std::string& terminator) {
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "B³¹d: Port szeregowy nie jest otwarty." << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> lock(serialMutex);
    isSending = true;

    // Prepare data with terminator
    std::vector<unsigned char> dataToSend = data;

    // Add terminator if specified
    for (char c : terminator) {
        dataToSend.push_back(static_cast<unsigned char>(c));
    }

    DWORD bytesWritten = 0;

    // Write data to serial port
    bool success = WriteFile(hSerial, dataToSend.data(), dataToSend.size(), &bytesWritten, NULL);

    if (!success) {
        std::cerr << "B³¹d podczas wysy³ania danych binarnych: " << GetLastError() << std::endl;
        isSending = false;
        return false;
    }

    // Ensure all data was sent
    bool result = (bytesWritten == dataToSend.size());

    if (result) {
        std::cout << "Wys³ano dane binarne: " << bytesWritten << " bajtów" << std::endl;
    }
    else {
        std::cerr << "Niepe³ne wys³anie danych binarnych: " << bytesWritten << " z " << dataToSend.size() << " bajtów." << std::endl;
    }

    isSending = false;
    return result;
}

void Sender::send() {
    // This would be implemented differently in a real application
    // with user interface integration
    std::cout << "Gotowy do wysy³ania danych..." << std::endl;
}

bool Sender::isCurrentlySending() const {
    return isSending;
}

bool Sender::isReady() const {
    return (hSerial != INVALID_HANDLE_VALUE);
}

void Sender::close() {
    std::lock_guard<std::mutex> lock(serialMutex);
    // Note: Do not close the handle here, as it's managed by PortManager
    hSerial = INVALID_HANDLE_VALUE;
}