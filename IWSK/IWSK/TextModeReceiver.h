#pragma once
#include <windows.h>
#include <string>

class TextModeReceiver {
public:
    TextModeReceiver();
    void receiveText();
    std::wstring receiveTextToString(HWND hwnd);
private:
    static const int BUFFER_SIZE = 256;
};