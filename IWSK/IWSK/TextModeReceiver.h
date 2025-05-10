#pragma once
#include <windows.h>
#include <string>
#include "PortManager.h"
#include <iostream>
#include <conio.h>  // dla _kbhit() i _getch()

class TextModeReceiver {
public:
    TextModeReceiver();
    void receiveText();
private:
    static const int BUFFER_SIZE = 256;
};