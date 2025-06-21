#include <algorithm>
#include <iostream>
#include "Terminator.h"

Terminator::Terminator(std::string chars) : terminator(chars), index(0) {}

bool Terminator::check(char in) {

	if (in == terminator[index]) index++;

	else {
		index = 0;
		return false;
	}

	if (index == terminator.size() + 1) {
		index = 0;
		return true;
	}

	return false;
}

std::string Terminator::get() const {
	return terminator;
}