#pragma once
#include <string>

class Terminator {
public:
	Terminator(std::string chars);

	bool check(char in);

	std::string get() const;

private:
	std::string terminator;
	size_t index;
};