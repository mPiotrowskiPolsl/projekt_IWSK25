#pragma once
#include <windows.h>
#include <string>

class TextModeReceiver {
public:
    TextModeReceiver();
    void receiveText();
private:
    static const int BUFFER_SIZE = 256;
};