#include "Ping.h"

bool PingChecker::ping(HANDLE hPort, char testByte, int timeoutMs) {
    if (hPort == INVALID_HANDLE_VALUE) {
        std::cerr << "B³êdny Handle portu" << std::endl;
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hPort, &testByte, 1, &bytesWritten, NULL) || bytesWritten != 1) {
        std::cerr << "B³¹d zapisu do portu" << std::endl;
        return false;
    }

    DWORD bytesRead;
    char response;
    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - startTime < std::chrono::milliseconds(timeoutMs)) {
        if (ReadFile(hPort, &response, 1, &bytesRead, NULL) && bytesRead == 1) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cerr << "Timeout" << std::endl;
    return false;
}
