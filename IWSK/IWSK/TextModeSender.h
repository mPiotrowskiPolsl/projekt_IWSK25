#pragma once

#include <windows.h>
#include <string>

class TextModeSender {
public:
	void setTerminator(const std::string& t);
	void sendText(HANDLE handle);

private:
	std::string terminator;
};
