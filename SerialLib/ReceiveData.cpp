#include "serialLib.h"

namespace Serial {
    bool SerialLib::receiveData(string &dst, uint32_t timeout) {
        DWORD commModemStatus;
        char *buff(nullptr);
        DWORD bufferLength(0);
        DWORD bufferLLengthFromOverlapped(0);
        OVERLAPPED overlappedRXEvent = {0};
        OVERLAPPED overlappedReadEvent = {0};
        COMSTAT portInfo = {0};
        DWORD nRead(0);

        memset(&overlappedRXEvent, 0, sizeof(overlappedRXEvent));
        memset(&overlappedReadEvent, 0, sizeof(overlappedReadEvent));

        overlappedRXEvent.hEvent = CreateEvent(nullptr, true, false, nullptr);
        overlappedReadEvent.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (!overlappedRXEvent.hEvent) {
            cerr << "ReceiveData Failed" << endl;
            return false;
        }

        if (!overlappedReadEvent.hEvent) {
            cerr << "ReceiveData Failed" << endl;
            return false;
        }

        if (!stream) {
            return false;
        }

        //EV_RXCHAR indicates that character was placed to input buffer
        SetCommMask(stream, EV_RXCHAR);
        WaitCommEvent(stream, &commModemStatus, &overlappedRXEvent);

        //suspend execution till EV_RXCHAR will fire
        switch (WaitForSingleObject(overlappedRXEvent.hEvent, timeout == 0 ? INFINITE : timeout)) {
            case WAIT_OBJECT_0:
                //Check is waiting for even completed successfully
                if (!GetOverlappedResult(stream, &overlappedRXEvent, &bufferLLengthFromOverlapped, false)) {
                    cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                    return false;
                }
                //Retrieve info from comm port
                ClearCommError(stream, nullptr, &portInfo);
                bufferLength = portInfo.cbInQue;
                buff = new char[bufferLength]; // +1 reservation for '\0' character
                //Read data from I/O buffer, fill osReader with event
                if (!ReadFile(stream, buff, bufferLength, &nRead, &overlappedReadEvent)) {
                    if (GetLastError() != ERROR_IO_PENDING)
                        cout << "Failed to read the stream errno " << GetLastError() << endl;
                }
                // suspend till ReadFile operation will complete its job
                if (WaitForSingleObject(overlappedReadEvent.hEvent, 200) == WAIT_OBJECT_0) {
                    if (!GetOverlappedResult(stream, &overlappedReadEvent, &bufferLLengthFromOverlapped, 200)) {
                        cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                    } else {
                        if (nRead > 0) {
                            buff[bufferLength] = '\0';
                            dst = buff;
                        } else return false;
                    }
                } else {
                    cerr << "Failed to read I/O buffer" << endl;
                    return false;
                }
                break;
            case WAIT_TIMEOUT:
                //process background info
                return false;
            default:
                cerr << "Error occurred in comm event" << endl;
                return false;
        }
        return true;
    }
}