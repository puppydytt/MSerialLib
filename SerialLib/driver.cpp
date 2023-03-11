#include "serialLib.h"

namespace Serial {
    bool SerialLib::openStream() {
        stream = CreateFile(iFaceName.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,      //  must be opened with exclusive-access
                            nullptr,   //  default security attributes
                            OPEN_EXISTING, //  must use OPEN_EXISTING
                            FILE_FLAG_OVERLAPPED,      //overlapped I/O
                            nullptr); //  hTemplate must be NULL for comm devices

        if (stream == INVALID_HANDLE_VALUE) return false;

        return true;
    }


    bool SerialLib::closeStream(const string &port) {
        if (stream) {

        } else return false;
        return false;
    }

    void SerialLib::setControl(DWORD baud, BYTE parity) {
        if (!stream) return;
        GetCommState(stream, &controlBlock);
        controlBlock.BaudRate = baud;
        controlBlock.fParity = parity;
        controlBlock.StopBits = 1;
        controlBlock.ByteSize = 8;
        SetCommState(stream, &controlBlock);
        /*GetCommTimeouts(stream, &timeouts);
        timeouts.ReadIntervalTimeout = timeout;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 0;
        SetCommTimeouts(stream, &timeouts);*/
    }
}
