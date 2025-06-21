#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <windows.h>

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


Receiver receiver;        // obiekt odbiornika
HWND hEdit2 = nullptr;    // uchwyt do pola tekstowego odbioru

#define WM_UPDATE_TEXT (WM_USER + 1)   // własny komunikat do aktualizacji tekstu
std::wstring receivedText;            // przechowa tekst odebrany w tle


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
PortManager portmanager; //zmienna globalna :(

class SerialCommunicationApp {
public:
    int run(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow) {

////////////////////////////////////////////
       // PortManager portManager;
       // if (!portManager.selectPort()) {
       //     std::cerr << "Brak dostępnego portu!" << std::endl;
       //     return 0;
       // }
       // portManager.configureSpeed();
       // portManager.configureFrame();

       // std::string port;
       // std::cout << "Enter COM port (e.g., COM1, COM2): ";
       // std::cin >> port;

       // FlowControl flowControl(port);
       // flowControl.setup();

       // if (flowControl.isManualMode()) {
       //     flowControl.manualFlow();
       // }

       // int choice;
       // std::cout << "\n=== Menu opcji ===" << std::endl;
       // std::cout << "1. Odbiór w trybie standardowym" << std::endl;
       // std::cout << "2. Odbiór w trybie tekstowym" << std::endl;
       // std::cout << "3. Nadawanie w trybie tekstowym." << std::endl;
       // std::cout << "4. Odbiór w trybie binarnym" << std::endl;std::cout << "Wybierz opcję: ";
       // std::cin >> choice;

       // if (choice == 1) {
       //     // Standardowy odbiór
       //     Receiver receiver;
       //     receiver.receive();
       // }
       // else if (choice == 2) {
       //     // Odbiór w trybie tekstowym
       //     TextModeReceiver textReceiver;
       //     textReceiver.receiveText();
       // }
       // else if (choice == 3) {
       //     // Nadawanie w trybie tekstowym
       //     Terminator* terminator = chooseTerminator();
       //     std::string terminatorText = terminator ? terminator->get() : "";

       //     TextModeSender sender;
       //     sender.setTerminator(terminatorText);
       //     std::cin.ignore();
       //     sender.sendText(portManager.getHandle());

       //     delete terminator;
       // }
       // else if (choice == 4) {
       //     BinaryModeReceiver binaryReceiver;

       //     int binChoice;
       //     std::cout << "\n--- Tryb binarny ---\n";
       //     std::cout << "1. Nadawanie z konsoli (hex)\n";
       //     std::cout << "2. Nadawanie z pliku\n";
       //     std::cout << "3. Odbiór danych binarnych\n";
       //     std::cout << "Twój wybór: ";
       //     std::cin >> binChoice;
       //     std::cin.ignore();

       //     if (binChoice == 1) {
       //         binaryReceiver.sendBinary();
       //     }
       //     else if (binChoice == 2) {
       //         std::string path;
       //         std::cout << "Podaj nazwę pliku do wysłania: ";
       //         std::getline(std::cin, path);
       //         binaryReceiver.sendFile(path);
       //     }
       //     else if (binChoice == 3) {
       //         binaryReceiver.receiveBinary();
       //     }
       //     else {
       //         std::cout << "Nieprawidłowy wybór trybu binarnego.\n";
       //     }
       // }
       //else {
       //     std::cout << "Nieprawidłowy wybór." << std::endl;
       // }
        ////////////////////////////////////////////
        
        const char CLASS_NAME[] = "MyWindowClass";

        WNDCLASS wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"MyWindowClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);
        int width = 800;
        int height = 600;
        HWND hwnd = CreateWindowEx(
            0,
            L"MyWindowClass",
            L"Moje Okno WinAPI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            NULL, NULL, hInstance, NULL
        );
        HWND hEdit = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            10, 10, width / 2 - 20, 200,
            hwnd, (HMENU)3001, hInstance, NULL);
        hEdit2 = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT | ES_READONLY,
            width / 2 + 10, 10, width / 2 - 20, 200,
            hwnd, (HMENU)3002, hInstance, NULL);


        //Lista Portów, odbywa sie w wndproc tak wlasciwie
        std::vector<std::string> ports;
        try {

            ports = portmanager.getAvailablePorts();
        }
        catch (std::runtime_error e) {
            
            MessageBox(hwnd, L"Brak dostępu do rej systemowego", L"Info", MB_OK);
            //tu ma byc normalnie wiadomosc z wyjatku |todo
        }
        for (int i = 0; i < ports.size(); i++) {
            std::wstring widestr = std::wstring(ports[i].begin(), ports[i].end());
            const wchar_t* widecstr = widestr.c_str();
            int id = 101+i;
            if (i == 0) {
                CreateWindowW(L"BUTTON", widecstr,
                    WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
                    20, 250+i*40, 100, 30, hwnd, (HMENU)id, hInstance, NULL);
                //pierwszy element z WS_GROUP
            }
            else {
                CreateWindowW(L"BUTTON", widecstr,
                    WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                    20, 250 + i * 40, 100, 30, hwnd, (HMENU)id, hInstance, NULL);
            }

        }
        //////

        //Wybór trybu transmisji
        CreateWindowW(L"BUTTON", L"Nadawanie tryb standardowy",
            WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
            150, 250, 220, 30, hwnd, (HMENU)201, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbiór tryb standardowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            150, 280, 220, 30, hwnd, (HMENU)202, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Nadawanie tryb binarny",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            150, 310, 220, 30, hwnd, (HMENU)203, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbiór tryb binarny",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            150, 340, 220, 30, hwnd, (HMENU)204, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Nadawanie tryb tekstowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            150, 370, 220, 30, hwnd, (HMENU)205, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbiór tryb tekstowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            150, 400, 220, 30, hwnd, (HMENU)206, hInstance, NULL);
        //////

        CreateWindowW(L"BUTTON", L"Sprawdź",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            10, height - 80, 100, 30, hwnd, (HMENU)200, hInstance, NULL);


        if (hwnd == NULL)
            return 0;

        ShowWindow(hwnd, nCmdShow);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return 0;
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

//int main() {
//    SerialCommunicationApp app;
//    app.run();
//    return 0;
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    AllocConsole();

    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpOut, "CONOUT$", "w", stderr);

    SerialCommunicationApp app;
    return app.run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}





// Procedura okna
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    

    std::string portNR = std::to_string(LOWORD(wParam));
    std::wstring widestr = std::wstring(portNR.begin(), portNR.end());
    const wchar_t* widecstr = widestr.c_str();

    switch (msg) {
    /*case WM_COMMANDDDD:

        

        MessageBox(hwnd, widecstr, L"Info", MB_OK);
        /*if (LOWORD(wParam) == 200) {
            if (SendMessage(GetDlgItem(hwnd, 101), BM_GETCHECK, 0, 0) == BST_CHECKED)
                MessageBox(hwnd, L"Wybrano opcję 1", L"Info", MB_OK);
            else if (SendMessage(GetDlgItem(hwnd, 102), BM_GETCHECK, 0, 0) == BST_CHECKED)
                MessageBox(hwnd, L"Wybrano opcję 2", L"Info", MB_OK);
            else if (SendMessage(GetDlgItem(hwnd, 103), BM_GETCHECK, 0, 0) == BST_CHECKED)
                MessageBox(hwnd, L"Wybrano opcję 3", L"Info", MB_OK);
        }
        if (LOWORD(wParam) < 200 && LOWORD(wParam) > 100) {
            portmanager.selectPort(LOWORD(wParam) - 101);
            //MessageBox(hwnd, L"Wybrano opcję 3", L"Info", MB_OK);
        }
        break;*/


    case WM_COMMAND: {
        int id = LOWORD(wParam);

        if (id == 200) {
            if (SendMessage(GetDlgItem(hwnd, 202), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                std::thread t([hwnd]() {
                    std::string result = receiver.receive();                         // odbiór danych
                    receivedText = std::wstring(result.begin(), result.end());       // zapis do globalnej zmiennej
                    PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);                          // wyślij komunikat do GUI
                    });
                t.detach();  // uruchom jako wątek niezależny
            }
        }


        if (id > 100 && id < 200) {
            portmanager.selectPort(id - 101);
        }

        break;
    }


    case WM_UPDATE_TEXT:
        if (hEdit2 != nullptr) {
            SetWindowTextW(hEdit2, receivedText.c_str());  // ustaw tekst w polu EDIT
        }
        break;



    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
