#pragma once
#include <windows.h>
#include <string>

class TextModeSender {
public:
    TextModeSender();
    std::wstring sendTextFromGUI(HANDLE handle, const std::string& message, const std::string& terminator);
};