#ifndef SENDER_H
#define SENDER_H

#include <string>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>

class Sender {
private:
    HANDLE hSerial;
    std::mutex serialMutex;
    std::atomic<bool> isSending;

public:
    Sender();

    // Initialize the sender with an existing serial port handle
    void initialize(HANDLE serialHandle);

    // Send text data with specified terminator
    bool send(const std::string& data, const std::string& terminator = "");

    // Send binary data with specified terminator
    bool sendBinary(const std::vector<unsigned char>& data, const std::string& terminator = "");

    // For the interface in your SerialCommunicationApp
    void send();

    // Check if sending is in progress
    bool isCurrentlySending() const;

    // Check if serial port is ready
    bool isReady() const;

    // Clean up resources
    void close();
};

#endif // SENDER_H