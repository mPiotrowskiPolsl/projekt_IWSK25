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

Terminator* chooseTerminator() {
	int choice;

	std::cout << "Wybierz terminator:\n";
	std::cout << "1) Brak\n";
	std::cout << "2) CR\n";
	std::cout << "3) LF\n";
	std::cout << "4) CR-LF\n";
	std::cout << "5) Niestandardowy\n";

	while (true) {
		std::cout << "Wybor: ";
		std::cin >> choice;
		std::cin.ignore();
		std::string t;
		switch (choice) {
		case 1:
			return nullptr;
		case 2:
			return new Terminator("\r");
		case 3:
			return new Terminator("\n");
		case 4:
			return new Terminator("\r\n");
		case 5:
			std::cout << "Podaj ciag terminatora: ";
			std::cin >> t;
			if (t.size() > 2) t = t.substr(0, 2);
			return new Terminator(t);
		default:
			std::cout << "Nieprawidlowy wybor";
		}
	}
}