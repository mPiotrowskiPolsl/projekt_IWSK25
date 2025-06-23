#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <windows.h>
#include <string>

class FlowControl {
public:
    FlowControl(const std::string& portName);
    ~FlowControl();

    void setup();
    void setMode(char mode);
    void manualFlow();

    bool isManualMode() const;

    bool writeData(const std::string& data);
    std::string readData();

    void setDTR(bool state);
    void setRTS(bool state);
    bool getDSR() const;
    bool getCTS() const;

private:
    enum class Mode { NONE, HARDWARE, SOFTWARE, MANUAL };
    Mode flowMode;

    HANDLE serialHandle;
    std::string portName;

    void enableHardwareFlowControl();
    void enableSoftwareFlowControl();
    
};

#endif // FLOWCONTROL_H
