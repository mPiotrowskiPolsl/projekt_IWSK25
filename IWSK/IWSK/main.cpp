#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <windows.h>
#include <commdlg.h>

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

static HWND hEditBin = NULL;
static WNDPROC OldEditProc = NULL;
LRESULT CALLBACK HexEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#define BINARY_MODE_MAX_SIZE 1024
WCHAR binaryEditBuffer[BINARY_MODE_MAX_SIZE]{};

#define WM_UPDATE_TEXT (WM_USER + 1)
// własny komunikat do aktualizacji tekstu
std::wstring receivedText;            // przechowa tekst odebrany w tle


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
PortManager portmanager; //zmienna globalna :(
FlowControl* flowControl = nullptr;
HWND hBtnDtrOn = NULL;
HWND hBtnDtrOff = NULL;
HWND hBtnRtsOn = NULL;
HWND hBtnRtsOff = NULL;
HWND hBtnDsr = NULL;
HWND hBtnCts = NULL;


BinaryModeReceiver binReceiver;
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
        int width = 1100;
        int height = 700;
        HWND hwnd = CreateWindowEx(
            0,
            L"MyWindowClass",
            L"Komunikacja rs232",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, hInstance, NULL
        );

        HWND hEdit = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            10, 10, width / 3 - 20, 200,
            hwnd, (HMENU)3001, hInstance, NULL);
        hEditBin = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            width / 3 + 10, 10, width / 3 - 20, 200,
            hwnd, (HMENU)3010, hInstance, NULL);
        OldEditProc = (WNDPROC)SetWindowLongPtr(hEditBin, GWLP_WNDPROC, (LONG_PTR)HexEditProc);
        hEdit2 = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT | ES_READONLY,
            2 * width / 3 + 10, 10, width / 3 - 30, 200,
            hwnd, (HMENU)3002, hInstance, NULL);


        /*CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            10, 500, 400, 25, hwnd, (HMENU)3003, hInstance, NULL);*/


        CreateWindowW(L"BUTTON", L"Wyslij",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            780, height - 80, 100, 30, hwnd, (HMENU)3005, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Wybierz i wyslij plik binarny",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            490, height-80, 280, 30, hwnd, (HMENU)3006, hInstance, NULL);

        //Lista Portów, odbywa sie w wndproc tak wlasciwie
        std::vector<std::string> ports;
        try {

            ports = portmanager.getAvailablePorts();
        }
        catch (std::runtime_error e) {

            MessageBox(hwnd, L"Brak dostepu do rej systemowego", L"Info", MB_OK);
            //tu ma byc normalnie wiadomosc z wyjatku |todo
        }

        int controlsY = 280;
		int portsYStep = 30;
		int portsX = 20;
        HWND hGroup = CreateWindowEx(0, L"BUTTON", L"Port",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            portsX-1, controlsY-20, 103, 350/*ports.size()*portsYStep+30*/, hwnd, NULL, hInstance, NULL);

        for (int i = 0; i < ports.size(); i++) {
            std::wstring widestr = std::wstring(ports[i].begin(), ports[i].end());
            const wchar_t* widecstr = widestr.c_str();
            int id = 101 + i;
            if (i == 0) {
                CreateWindowW(L"BUTTON", widecstr,
                    WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
                    portsX, controlsY + i * portsYStep, 100, 30, hwnd, (HMENU)id, hInstance, NULL);
                //pierwszy element z WS_GROUP
            }
            else {
                CreateWindowW(L"BUTTON", widecstr,
                    WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                    portsX, controlsY + i * portsYStep, 100, 30, hwnd, (HMENU)id, hInstance, NULL);
            }

        }
        //////
        
        int transSelectionYStep = 30;
        int transSelectionX = 125;
        //Wybór trybu transmisji
        HWND hGroup2 = CreateWindowEx(0, L"BUTTON", L"Tryb transmisji",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            transSelectionX - 1, controlsY - 20, 223, 350/*ports.size()*portsYStep+30*/, hwnd, NULL, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Nadawanie tryb standardowy",
            WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 0, 220, 30, hwnd, (HMENU)201, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbior tryb standardowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 1, 220, 30, hwnd, (HMENU)202, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Nadawanie tryb binarny",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 2, 220, 30, hwnd, (HMENU)203, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbior tryb binarny",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 3, 220, 30, hwnd, (HMENU)204, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Nadawanie tryb tekstowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 4, 220, 30, hwnd, (HMENU)205, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Odbior tryb tekstowy",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 5, 220, 30, hwnd, (HMENU)206, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Pingowanie",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            transSelectionX, controlsY + transSelectionYStep * 6, 100, 30, hwnd, (HMENU)207, hInstance, NULL);

        //////

		int terminatorYStep = 30;
        int terminatorX = 350;
        //Wybór terminatora
        HWND hGroup3 = CreateWindowEx(0, L"BUTTON", L"Wybor terminatora",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            terminatorX - 1, controlsY - 20, 203, 350/*ports.size()*portsYStep+30*/, hwnd, NULL, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Dodaj terminator",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            terminatorX, controlsY + terminatorYStep * 0, 150, 30, hwnd, (HMENU)3004, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"CR",
            WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
            terminatorX, controlsY + terminatorYStep * 1, 150, 30, hwnd, (HMENU)301, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"LF",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            terminatorX, controlsY + terminatorYStep * 2, 150, 30, hwnd, (HMENU)302, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"CR-LF",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            terminatorX, controlsY + terminatorYStep * 3, 150, 30, hwnd, (HMENU)303, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Niestandardowy:",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            terminatorX, controlsY + terminatorYStep * 4, 150, 30, hwnd, (HMENU)304, hInstance, NULL);
        CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            terminatorX+150, controlsY + terminatorYStep * 4, 50, 25, hwnd, (HMENU)305, hInstance, NULL);
        //////

        CreateWindowW(L"BUTTON", L"Odbierz",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            10, height - 80, 100, 30, hwnd, (HMENU)200, hInstance, NULL);
        CreateWindowW(L"BUTTON", L"Wyswietlaj",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            220, controlsY + transSelectionYStep * 6, 100, 30, hwnd, (HMENU)401, hInstance, NULL);

        //kontrola przeplywu
		int flowControlX = 555;
		
		int flowControlYStep = 40;
        HWND hGroup4 = CreateWindowEx(0, L"BUTTON", L"Kontrola Przeplywu",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            flowControlX - 1, controlsY - 20, 223, 350/*ports.size()*portsYStep+30*/, hwnd, NULL, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Brak kontroli przeplywu",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            flowControlX, controlsY + flowControlYStep * 0, 220, 30, hwnd, (HMENU)701, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Sprzetowa kontrola przeplywu",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            flowControlX, controlsY + flowControlYStep * 1, 220, 30, hwnd, (HMENU)702, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Programowa kontrola przeplywu",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            flowControlX, controlsY + flowControlYStep * 2, 220, 30, hwnd, (HMENU)703, hInstance, NULL);

        CreateWindowW(L"BUTTON", L"Tryb reczny (DTR/RTS)",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            flowControlX, controlsY + flowControlYStep * 3, 220, 30, hwnd, (HMENU)704, hInstance, NULL);

        
        hBtnDtrOn = CreateWindowW(L"BUTTON", L"DTR ON",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX, controlsY + flowControlYStep * 4, 100, 30, hwnd, (HMENU)705, hInstance, NULL);

        hBtnDtrOff = CreateWindowW(L"BUTTON", L"DTR OFF",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX+120, controlsY + flowControlYStep * 4, 100, 30, hwnd, (HMENU)706, hInstance, NULL);

        hBtnRtsOn = CreateWindowW(L"BUTTON", L"RTS ON",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX, controlsY + flowControlYStep * 5, 100, 30, hwnd, (HMENU)707, hInstance, NULL);

        hBtnRtsOff = CreateWindowW(L"BUTTON", L"RTS OFF",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX+120, controlsY + flowControlYStep * 5, 100, 30, hwnd, (HMENU)708, hInstance, NULL);

        hBtnDsr = CreateWindowW(L"BUTTON", L"Sprawdz DSR",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX, controlsY + flowControlYStep * 6, 100, 30, hwnd, (HMENU)709, hInstance, NULL);

        hBtnCts = CreateWindowW(L"BUTTON", L"Sprawdz CTS",
            WS_CHILD | WS_VISIBLE | WS_DISABLED,
            flowControlX+120, controlsY + flowControlYStep * 6, 100, 30, hwnd, (HMENU)710, hInstance, NULL);




        //Parametry transmiji
		int transmissionParamsX = 780;
        HWND hGroup5 = CreateWindowEx(0, L"BUTTON", L"Parametry transmisji",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            transmissionParamsX - 1, controlsY - 20, 283, 350/*ports.size()*portsYStep+30*/, hwnd, NULL, hInstance, NULL);
        //TODO



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
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            const char* wej = "Wejscie";
            const char* wejHex = "Wejscie hex";
            const char* wyj = "Wyjscie";
            TextOutA(hdc, 10, 215, wej, lstrlenA(wej)); // podpis pod wejsciem
            TextOutA(hdc, 310, 215, wejHex, lstrlenA(wejHex)); // podpis pod wejsciem
            TextOutA(hdc, 610, 215, wyj, lstrlenA(wyj)); // podpis pod wejsciem

            EndPaint(hwnd, &ps);
        }
        return 0;
        //break;

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


		case WM_COMMAND: {// obsługa komunikatów z przycisków i innych elementów GUI
        int id = LOWORD(wParam);

        if (id == 200) {//200 - przycisk sprawdz
            if (SendMessage(GetDlgItem(hwnd, 202), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                std::thread t([hwnd]() {
                    std::string result = receiver.receive();                         // odbiór danych
                    receivedText = std::wstring(result.begin(), result.end());       // zapis do globalnej zmiennej
                    PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);                          // wyślij komunikat do GUI
                    });
                t.detach();  // uruchom jako wątek niezależny
            }
            else if (SendMessage(GetDlgItem(hwnd, 204), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                std::thread t([hwnd]() {
                    //binReceiver;
                    receivedText = binReceiver.receiveBinaryToString();
                    PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);
                    });
                t.detach();
            }
            else if (SendMessage(GetDlgItem(hwnd, 206), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                // Odbior tryb tekstowy - NOWE
                std::thread t([hwnd]() {
                    TextModeReceiver textReceiver;
                    textReceiver.receiveTextToString(hwnd);
                    });
                t.detach();
            }
        }
		if (id == 207) {//pingowanie
            std::thread t([hwnd]() {
                while (SendMessage(GetDlgItem(hwnd, 207), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    if (SendMessage(GetDlgItem(hwnd, 401), BM_GETCHECK, 0, 0) == BST_CHECKED) {//przycisk ping, niech dziala tylko jak jest zaznaczony
                        receivedText += PingChecker::ping(portmanager.getHandle()) + L"\n";
                        PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);
                         // wyjdz z petli, jesli nie jest zaznaczony
                    }
                    else {
                        PingChecker::ping(portmanager.getHandle());
                    }
                    Sleep(1000); // odczekaj 1 sekunde przed kolejnym pingiem
                }
                });
            t.detach();
        }
        if (id == 701 && flowControl) {
            flowControl->setMode('n');

            EnableWindow(hBtnDtrOn, FALSE);
            EnableWindow(hBtnDtrOff, FALSE);
            EnableWindow(hBtnRtsOn, FALSE);
            EnableWindow(hBtnRtsOff, FALSE);
            EnableWindow(hBtnDsr, FALSE);
            EnableWindow(hBtnCts, FALSE);
        }
        if (id == 702 && flowControl) {
            flowControl->setMode('h');

            EnableWindow(hBtnDtrOn, FALSE);
            EnableWindow(hBtnDtrOff, FALSE);
            EnableWindow(hBtnRtsOn, FALSE);
            EnableWindow(hBtnRtsOff, FALSE);
            EnableWindow(hBtnDsr, FALSE);
            EnableWindow(hBtnCts, FALSE);
        }
        if (id == 703 && flowControl) {
            flowControl->setMode('s');

            EnableWindow(hBtnDtrOn, FALSE);
            EnableWindow(hBtnDtrOff, FALSE);
            EnableWindow(hBtnRtsOn, FALSE);
            EnableWindow(hBtnRtsOff, FALSE);
            EnableWindow(hBtnDsr, FALSE);
            EnableWindow(hBtnCts, FALSE);
        }
        if (id == 704 && flowControl) {
            flowControl->setMode('m');

            EnableWindow(hBtnDtrOn, TRUE);
            EnableWindow(hBtnDtrOff, TRUE);
            EnableWindow(hBtnRtsOn, TRUE);
            EnableWindow(hBtnRtsOff, TRUE);
            EnableWindow(hBtnDsr, TRUE);
            EnableWindow(hBtnCts, TRUE);
        }

        if (id == 705 && flowControl) {
            flowControl->setDTR(true);
        }
        if (id == 706 && flowControl) {
            flowControl->setDTR(false);
        }
        if (id == 707 && flowControl) {
            flowControl->setRTS(true);
        }
        if (id == 708 && flowControl) {
            flowControl->setRTS(false);
        }
        if (id == 709 && flowControl) {
            bool dsr = flowControl->getDSR();
            MessageBox(hwnd, dsr ? L"DSR: HIGH" : L"DSR: LOW", L"Stan DSR", MB_OK);
        }
        if (id == 710 && flowControl) {
            bool cts = flowControl->getCTS();
            MessageBox(hwnd, cts ? L"CTS: HIGH" : L"CTS: LOW", L"Stan CTS", MB_OK);
        }
       
        BOOL isStandardModeSelected = SendMessage(GetDlgItem(hwnd, 201), BM_GETCHECK, 0, 0) == BST_CHECKED;
        BOOL isBinaryModeSelected = SendMessage(GetDlgItem(hwnd, 203), BM_GETCHECK, 0, 0) == BST_CHECKED;
        bool isTextModeSelected = SendMessage(GetDlgItem(hwnd, 205), BM_GETCHECK, 0, 0) == BST_CHECKED;
        
        if (id == 3005 && isStandardModeSelected) {
            wchar_t buffer[1024];
            GetWindowTextW(GetDlgItem(hwnd, 3001), buffer, 1024);
            std::wstring ws(buffer);
            std::string inputText(ws.begin(), ws.end());

            bool addTerminator = (SendMessage(GetDlgItem(hwnd, 3004), BM_GETCHECK, 0, 0) == BST_CHECKED);
            std::string terminator;
            if (addTerminator) {
                if (SendMessage(GetDlgItem(hwnd, 301), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\r";
                else if (SendMessage(GetDlgItem(hwnd, 302), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\n";
                else if (SendMessage(GetDlgItem(hwnd, 303), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\r\n";
                else if (SendMessage(GetDlgItem(hwnd, 304), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    wchar_t custom[3] = { 0 };
                    GetWindowTextW(GetDlgItem(hwnd, 305), custom, 2);
                    std::wstring wcustom(custom);
                    terminator = std::string(wcustom.begin(), wcustom.end());
                }
            }

            std::thread t([inputText, terminator]() {
                HANDLE handle = PortManager::getHandle();
                if (handle == INVALID_HANDLE_VALUE) {
                    std::cerr << "[Sender] Blad portu\n";
                    return;
                }
                Sender sender;
                sender.initialize(handle);
                sender.send(inputText, terminator);
            });
            t.detach();
        }
        
        if (id == 3005 && isTextModeSelected) {
            wchar_t buffer[1024];
            GetWindowTextW(GetDlgItem(hwnd, 3001), buffer, 1024);
            //SetWindowTextW(GetDlgItem(hwnd, 3001), buffer);

            std::wstring ws(buffer);
            std::string inputText(ws.begin(), ws.end());

            bool addTerminator = (SendMessage(GetDlgItem(hwnd, 3004), BM_GETCHECK, 0, 0) == BST_CHECKED);
            std::string terminator;

            if (addTerminator) {
                if (SendMessage(GetDlgItem(hwnd, 301), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\r";
                else if (SendMessage(GetDlgItem(hwnd, 302), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\n";
                else if (SendMessage(GetDlgItem(hwnd, 303), BM_GETCHECK, 0, 0) == BST_CHECKED)
                    terminator = "\r\n";
                else if (SendMessage(GetDlgItem(hwnd, 304), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    wchar_t custom[3] = { 0 };
                    GetWindowTextW(GetDlgItem(hwnd, 305), custom, 2);
                    std::wstring wcustom(custom);
                    terminator = std::string(wcustom.begin(), wcustom.end());
                }
            }

            std::thread t([hwnd, inputText, terminator]() {
                HANDLE handle = PortManager::getHandle();
                if (handle == INVALID_HANDLE_VALUE) {
                    std::cerr << "[TextModeSender] Blad portu\n";
                    return;
                }
                TextModeSender sender;
                std::wstring sendResult = sender.sendTextFromGUI(handle, inputText, terminator);
                MessageBox(hwnd, sendResult.c_str(), L"Info", MB_OK);
            });
            t.detach();
        }

        if (id == 3005 && isBinaryModeSelected) {
            bool addTerminator = (SendMessage(GetDlgItem(hwnd, 3004), BM_GETCHECK, 0, 0) == BST_CHECKED);

            Terminator* terminator = nullptr;

            if (addTerminator) {
                if (SendMessage(GetDlgItem(hwnd, 301), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\r");
                }
                else if (SendMessage(GetDlgItem(hwnd, 302), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\n");
                }
                else if (SendMessage(GetDlgItem(hwnd, 303), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\r\n");
                }
                else if (SendMessage(GetDlgItem(hwnd, 304), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    wchar_t buf[2];
                    GetWindowTextW(GetDlgItem(hwnd, 305), buf, 2);
                    std::wstring tmp(buf);
                    std::string sequence(tmp.begin(), tmp.end());
                    terminator = new Terminator("\r");
                }
            }

            GetWindowTextW(hEditBin, binaryEditBuffer, BINARY_MODE_MAX_SIZE);

            BinaryModeSender binaryModeSender(hwnd);
            if (terminator)
            {
                binaryModeSender.sendFromHex(hwnd, binaryEditBuffer, *terminator);
            }
            else
            {
                std::cerr << "Brak terminatora\n";
            }

            delete terminator;
        }

        if (id == 3006)
        {
            bool addTerminator = (SendMessage(GetDlgItem(hwnd, 3004), BM_GETCHECK, 0, 0) == BST_CHECKED);

            Terminator* terminator = nullptr;

            if (addTerminator) {
                if (SendMessage(GetDlgItem(hwnd, 301), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\r");
                }
                else if (SendMessage(GetDlgItem(hwnd, 302), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\n");
                }
                else if (SendMessage(GetDlgItem(hwnd, 303), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    terminator = new Terminator("\r\n");
                }
                else if (SendMessage(GetDlgItem(hwnd, 304), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    wchar_t buf[2];
                    GetWindowTextW(GetDlgItem(hwnd, 305), buf, 2);
                    std::wstring tmp(buf);
                    std::string sequence(tmp.begin(), tmp.end());
                    terminator = new Terminator("\r");
                }
            }

            if (terminator)
            {
                wchar_t szFile[260] = { 0 };

                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(ofn));

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = NULL;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);

                ofn.lpstrFilter = L"Wszystkie pliki\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn)) {
                    BinaryModeSender binaryModeSender(hwnd);
                    if (binaryModeSender.readAndSendFile(hwnd, ofn.lpstrFile, *terminator))
                    {
                        std::wstring helper = L"Wyslano plik: ";
                        helper += ofn.lpstrFile;
                        MessageBox(hwnd, helper.c_str(), L"Info", MB_OK);
                    }

                }
                else {
                    MessageBox(hwnd, L"Anulowano", L"Info", MB_OK);
                }

                delete terminator;
            }
            else
            {
                std::cout << "brak terminatowora";
            }

        }

        if (id > 100 && id < 200) {
            portmanager.selectPort(id - 101);

            if (flowControl) {
                delete flowControl;
                flowControl = nullptr;
            }

            std::string selectedPort = portmanager.getPort();
            flowControl = new FlowControl(selectedPort, portmanager.getHandle());
        }
        std::string selectedPort = portmanager.getPort();
        flowControl = new FlowControl(selectedPort, portmanager.getHandle());

        break;
    }


    case WM_UPDATE_TEXT:
        if (hEdit2 != nullptr) {
            SetWindowTextW(hEdit2, receivedText.c_str());  // ustaw tekst w polu EDIT
        }
        break;



    case WM_DESTROY:
        if (flowControl) {
            delete flowControl;
            flowControl = nullptr;
        }
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK HexEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CHAR:
        if ((wParam >= '0' && wParam <= '9') ||
            (wParam >= 'a' && wParam <= 'f') ||
            (wParam >= 'A' && wParam <= 'F') ||
            wParam == VK_BACK || wParam == VK_DELETE || wParam == VK_RETURN || wParam == '\t') {
            if (wParam >= 'a' && wParam <= 'f') {
                wParam -= 32;
            }
            return CallWindowProc(OldEditProc, hwnd, msg, wParam, lParam);
        }
        // Beep or ignore invalid char
        MessageBeep(MB_ICONWARNING);
        return 0;
    }
    return CallWindowProc(OldEditProc, hwnd, msg, wParam, lParam);
}