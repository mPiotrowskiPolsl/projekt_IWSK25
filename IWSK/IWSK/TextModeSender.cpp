#include "TextModeSender.h"
#include <iostream>

void TextModeSender::setTerminator(const std::string& t) {
    terminator = t;
}

void TextModeSender::sendText(HANDLE handle) {
	if (handle == INVALID_HANDLE_VALUE) {
		std::cerr << "[TextModeSender] Nieprawid³owy uchwyt portu COM!" << std::endl;
		return;
	}
	std::string inputText;

	std::cout << "\n=== Tryb transmisji tekstowy - NADAWANIE ===" << std::endl;

	//Wprowadzenie tekstu
	std::cout << "WprowadŸ tekst do wys³ania" << std::endl;
	std::getline(std::cin, inputText);


    // 3. Prezentacja bufora
    std::cout << "\n[Bufor] Tekst do wys³ania:\n" << inputText << std::endl;
    std::cout << "[Bufor] Terminator: ";
    if (terminator.empty()) {
        std::cout << "(brak)";
    }
    else {
        for (char c : terminator) {
            if (c == '\r') std::cout << "\\r";
            else if (c == '\n') std::cout << "\\n";
            else std::cout << c;
        }
    }
    std::cout << std::endl;

    // 4. Potwierdzenie wysy³ki
    std::string decision;
    std::cout << "\nWpisz „wyslij”, aby nadaæ dane, lub cokolwiek innego, aby anulowaæ: ";
    std::getline(std::cin, decision);

    if (decision != "wyslij") {
        std::cout << "[TX] Anulowano nadawanie." << std::endl;
        return;
    }

    // 5. Wysy³anie przez port
    std::string fullMessage = inputText + terminator;
    DWORD bytesWritten;
    BOOL result = WriteFile(
        handle,
        fullMessage.c_str(),
        static_cast<DWORD>(fullMessage.size()),
        &bytesWritten,
        NULL
    );

    if (!result) {
        std::cerr << "[TextModeSender] B³¹d podczas wysy³ania (kod: " << GetLastError() << ")" << std::endl;
    }
    else {
        std::cout << "[TX] Wys³ano " << bytesWritten << " bajtów." << std::endl;
    }
}
