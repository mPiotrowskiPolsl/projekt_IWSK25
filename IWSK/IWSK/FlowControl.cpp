#include "FlowControl.h"
#include <iostream>
#include <thread>
#include <chrono>

FlowControl::FlowControl(const std::string& port) : portName(port), flowMode(Mode::NONE) {
    serialHandle = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (serialHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open serial port: " << portName << std::endl;
    }
}

FlowControl::~FlowControl() {
    if (serialHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(serialHandle);
    }
}

void FlowControl::setup() {
    char choice;
    std::cout << "Select flow control mode:\n"
        << "n - None\n"
        << "h - Hardware (RTS/CTS, DTR/DSR)\n"
        << "s - Software (XON/XOFF)\n"
        << "m - Manual\n"
        << "Choice: ";
    std::cin >> choice;

    switch (choice) {
    case 'n':
        flowMode = Mode::NONE;
        std::cout << "No flow control selected." << std::endl;
        break;
    case 'h':
        flowMode = Mode::HARDWARE;
        enableHardwareFlowControl();
        break;
    case 's':
        flowMode = Mode::SOFTWARE;
        enableSoftwareFlowControl();
        break;
    case 'm':
        flowMode = Mode::MANUAL;
        std::cout << "Manual flow control enabled." << std::endl;
        break;
    default:
        std::cout << "Invalid selection. No flow control applied." << std::endl;
        flowMode = Mode::NONE;
    }
}

void FlowControl::enableHardwareFlowControl() {
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(serialHandle, &dcbSerialParams)) {
        std::cerr << "Error getting COM state" << std::endl;
        return;
    }

    dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;  // Enable RTS/CTS handshake
    dcbSerialParams.fDtrControl = DTR_CONTROL_HANDSHAKE;  // Enable DTR/DSR handshake

    if (!SetCommState(serialHandle, &dcbSerialParams)) {
        std::cerr << "Error setting hardware flow control" << std::endl;
    }
}

void FlowControl::enableSoftwareFlowControl() {
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(serialHandle, &dcbSerialParams)) {
        std::cerr << "Error getting COM state" << std::endl;
        return;
    }

    dcbSerialParams.fOutX = TRUE;  // Enable XON/XOFF
    dcbSerialParams.fInX = TRUE;

    if (!SetCommState(serialHandle, &dcbSerialParams)) {
        std::cerr << "Error setting software flow control" << std::endl;
    }
}

bool FlowControl::isManualMode() const {
    return flowMode == Mode::MANUAL;
}

void FlowControl::manualFlow() {
    if (!isManualMode()) return;

    char command;
    while (true) {
        std::cout << "Manual control options:\n"
            << "1 - Set DTR high\n"
            << "2 - Set DTR low\n"
            << "3 - Set RTS high\n"
            << "4 - Set RTS low\n"
            << "5 - Check DSR state\n"
            << "6 - Check CTS state\n"
            << "q - Quit manual mode\n"
            << "Choice: ";
        std::cin >> command;

        switch (command) {
        case '1': setDTR(true); break;
        case '2': setDTR(false); break;
        case '3': setRTS(true); break;
        case '4': setRTS(false); break;
        case '5': std::cout << "DSR state: " << (getDSR() ? "HIGH" : "LOW") << std::endl; break;
        case '6': std::cout << "CTS state: " << (getCTS() ? "HIGH" : "LOW") << std::endl; break;
        case 'q': return;
        default: std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void FlowControl::setDTR(bool state) {
    EscapeCommFunction(serialHandle, state ? SETDTR : CLRDTR);
}

void FlowControl::setRTS(bool state) {
    EscapeCommFunction(serialHandle, state ? SETRTS : CLRRTS);
}

bool FlowControl::getDSR() const {
    DWORD status;
    if (GetCommModemStatus(serialHandle, &status)) {
        return status & MS_DSR_ON;
    }
    std::cerr << "Error reading DSR state" << std::endl;
    return false;
}

bool FlowControl::getCTS() const {
    DWORD status;
    if (GetCommModemStatus(serialHandle, &status)) {
        return status & MS_CTS_ON;
    }
    std::cerr << "Error reading CTS state" << std::endl;
    return false;
}
