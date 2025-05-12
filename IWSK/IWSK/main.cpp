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
//#include "PingChecker.h"          // Osoba 5
//#include "TextModeSender.h"       // Osoba 6
#include "BinaryModeSender.h"     // Osoby 8, 9
//#include "BinaryModeReceiver.h"   // Osoby 10, 11
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
        std::cout << "Wybierz opcjê: ";
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
        else {
            std::cout << "Nieprawid³owy wybór." << std::endl;
        }

        /*Terminator terminator;
        terminator.chooseTerminator();

        PingChecker ping;
        ping.checkConnection();
        ping.autoBaud();

        Sender sender;
        sender.send();

        TextModeSender textSender;
        textSender.sendText();*/

        BinaryModeSender binarySender;
        binarySender.readHexBytesFromConsole();
        binarySender.sendWithTerminator();
        binarySender.sendFile("nazwa pliku", 0);

        //BinaryModeReceiver binaryReceiver;
        //binaryReceiver.receiveBinary();
    }
};

int main() {
    SerialCommunicationApp app;
    app.run();
    return 0;
}