#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#include "PortManager.h"          // Osoba 1
#include "FlowControl.h"          // Osoba 2
#include "Terminator.h"           // Osoba 3
#include "Sender.h"               // Osoba 4
#include "TextModeReceiver.h"     // Osoba 7
#include "Ping.h"                 // Osoba 5
#include "TextModeSender.h"       // Osoba 6
#include "BinaryModeSender.h"     // Osoby 8, 9
#include "BinaryModeReceiver.h"   // Osoby 10, 11
#include "Receiver.h"             // Osoba 12

class SerialCommunicationApp {
public:
    void run() {
        PortManager portManager;
        if (!portManager.selectPort()) {
            std::cerr << "Brak dostêpnego portu!" << std::endl;
            return;
        }
        portManager.configureSpeed();
        portManager.configureFrame();

        std::string port;
        std::cout << "Enter COM port (e.g., COM1, COM2): ";
        std::cin >> port;

        FlowControl flowControl(port);
        flowControl.setup();

        if (flowControl.isManualMode()) {
            flowControl.manualFlow();
        }

        int choice;
        std::cout << "\n=== Menu opcji ===" << std::endl;
        std::cout << "1. Odbiór w trybie standardowym" << std::endl;
        std::cout << "2. Odbiór w trybie tekstowym" << std::endl;
        std::cout << "3. Nadawanie w trybie tekstowym." << std::endl;
        std::cout << "4. Odbiór w trybie binarnym" << std::endl;std::cout << "Wybierz opcjê: ";
        std::cin >> choice;

        if (choice == 1) {
            // Standardowy odbiór
            Receiver receiver;
            receiver.receive();
        }
        else if (choice == 2) {
            // Odbiór w trybie tekstowym
            TextModeReceiver textReceiver;
            textReceiver.receiveText();
        }
        else if (choice == 3) {
            // Nadawanie w trybie tekstowym
            Terminator* terminator = chooseTerminator();
            std::string terminatorText = terminator ? terminator->get() : "";

            TextModeSender sender;
            sender.setTerminator(terminatorText);
            std::cin.ignore();
            sender.sendText(portManager.getHandle());

            delete terminator;
        }
        else if (choice == 4) {
            BinaryModeReceiver binaryReceiver;

            int binChoice;
            std::cout << "\n--- Tryb binarny ---\n";
            std::cout << "1. Nadawanie z konsoli (hex)\n";
            std::cout << "2. Nadawanie z pliku\n";
            std::cout << "3. Odbiór danych binarnych\n";
            std::cout << "Twój wybór: ";
            std::cin >> binChoice;
            std::cin.ignore();

            if (binChoice == 1) {
                binaryReceiver.sendBinary();
            }
            else if (binChoice == 2) {
                std::string path;
                std::cout << "Podaj nazwê pliku do wys³ania: ";
                std::getline(std::cin, path);
                binaryReceiver.sendFile(path);
            }
            else if (binChoice == 3) {
                binaryReceiver.receiveBinary();
            }
            else {
                std::cout << "Nieprawid³owy wybór trybu binarnego.\n";
            }
        }
       else {
            std::cout << "Nieprawid³owy wybór." << std::endl;
        }

        /*Terminator terminator;
        terminator.chooseTerminator();
        *
        PingChecker ping;
        ping.ping(portManager.getHandle());
        ping.autoBaud(portManager.getHandle());
        /*
        Sender sender;
        sender.send();*/

       /* BinaryModeSender binarySender;
        binarySender.readHexBytesFromConsole();
        binarySender.sendWithTerminator();
        binarySender.sendFile("nazwa pliku", 0);

        BinaryModeReceiver binaryReceiver;
        binaryReceiver.receiveBinary();
        binaryReceiver.sendBinary();
        */
    }
};

int main() {
    SerialCommunicationApp app;
    app.run();
    return 0;
}