#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <windows.h>
#include <string>

class FlowControl {
public:
    FlowControl(const std::string& portName);
    ~FlowControl();

    void setup();          // Configure flow control mode
    void manualFlow();     // Manual control of DTR/RTS

    bool isManualMode() const;

private:
    enum class Mode { NONE, HARDWARE, SOFTWARE, MANUAL };
    Mode flowMode;

    HANDLE serialHandle;
    std::string portName;

    void enableHardwareFlowControl();  // RTS/CTS, DTR/DSR handshake
    void enableSoftwareFlowControl();  // XON/XOFF

    void setDTR(bool state);  // Set DTR signal manually
    void setRTS(bool state);  // Set RTS signal manually
    bool getDSR() const;      // Check DSR state
    bool getCTS() const;      // Check CTS state
};

#endif // FLOWCONTROL_H
