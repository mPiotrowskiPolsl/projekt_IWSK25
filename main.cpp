#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

// Pliki nagłówkowe dla poszczególnych modułów
#include "PortManager.h"          // Osoba 1
#include "FlowControl.h"          // Osoba 2
#include "Terminator.h"           // Osoba 3
#include "Sender.h"               // Osoba 4
#include "PingChecker.h"          // Osoba 5
#include "TextModeSender.h"       // Osoba 6
#include "TextModeReceiver.h"     // Osoba 7
#include "BinaryModeSender.h"     // Osoby 8, 9
#include "BinaryModeReceiver.h"   // Osoby 10, 11
#include "Receiver.h"             // Osoba 12

// Główna klasa aplikacji
class SerialCommunicationApp {
public:
    void run() {
        PortManager portManager;
        if (!portManager.selectPort()) {
            std::cerr << "Brak dostępnego portu!" << std::endl;
            return;
        }
        portManager.configureSpeed();
        portManager.configureFrame();

        FlowControl flowControl;
        flowControl.setup();
        flowControl.manualFlow();

        Terminator terminator;
        terminator.chooseTerminator();

        PingChecker ping;
        ping.checkConnection();
        ping.autoBaud();

        Sender sender;
        sender.send();

        Receiver receiver;
        receiver.receive();

        TextModeSender textSender;
        textSender.sendText();

        TextModeReceiver textReceiver;
        textReceiver.receiveText();

        BinaryModeSender binarySender;
        binarySender.sendBinary();

        BinaryModeReceiver binaryReceiver;
        binaryReceiver.receiveBinary();
    }
};

int main() {
    SerialCommunicationApp app;
    app.run();
    return 0;
}
