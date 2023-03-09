#include "serialLib.h"

//make normal reception
namespace Serial {
    bool SerialLib::receiveData(string &dst, uint32_t timeout, uint32_t fillBufferTimeout) {
        DWORD commModemStatus;
        char *buff(nullptr);
        DWORD bufferLength(0);
        DWORD bufferLLengthFromOverlapped(0);
        //separate overlapped structure for operation. See Remarks there https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-overlapped
        OVERLAPPED overlappedRXEvent = {0};
        OVERLAPPED overlappedReadEvent = {0};
        COMSTAT portInfo = {0};
        DWORD nRead(0);

        //init of overlapped structure
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

        eventRead = overlappedRXEvent.hEvent;

        //suspend execution till EV_RXCHAR will fire
        uint32_t offset = 0;
        switch (WaitForSingleObject(overlappedRXEvent.hEvent, timeout == 0 ? INFINITE : timeout)) {
            case WAIT_OBJECT_0:
                //Check is waiting for even completed successfully
                if (!GetOverlappedResult(stream, &overlappedRXEvent, &bufferLLengthFromOverlapped, false)) {
                    cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                    return false;
                }

                //thread sync, wait for write thread to complete its own operation
                WaitForSingleObject(eventWrite, INFINITE);

                //Retrieve info from comm port (bytes in input buffer + bytes in output buffer)
                while (true) {
                    ClearCommError(stream, nullptr, &portInfo);
                    bufferLength += portInfo.cbInQue;
                    if (portInfo.cbInQue == 0) break; // read all message from input buffer
                    buff = static_cast<char *>(malloc(bufferLength)); // +1 reservation for '\0' character TODO realloc?
                    //Read data from I/O buffer, fill osReader with eventRead
                    if (!ReadFile(stream, buff /*offset*/, portInfo.cbInQue, &nRead, //toDo readToBuffOfset?
                                  &overlappedReadEvent)) {
                        if (GetLastError() != ERROR_IO_PENDING)
                            cout << "Failed to read the stream errno " << GetLastError() << endl;
                    }
                    // suspend till ReadFile operation will complete its job
                    if (WaitForSingleObject(overlappedReadEvent.hEvent, INFINITE) == WAIT_OBJECT_0) {
                        if (!GetOverlappedResult(stream, &overlappedReadEvent, &bufferLLengthFromOverlapped,
                                                 INFINITE)) {
                            cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                        } else {
                            cout << buff << endl; //OK
                        }

                    } else {
                        cerr << "Failed to read I/O buffer" << endl;
                        return false;
                    }
                    offset += bufferLength;
                    if (fillBufferTimeout > 0) Sleep(fillBufferTimeout);
                }
                if (bufferLength != 0) buff[bufferLength] = '\0';
                break;
            case WAIT_TIMEOUT:
                //process background info
                return false;
            default:
                cerr << "Error occurred in comm event" << endl;
                return false;
        }
        //dst = buff;
        cout << bufferLength << endl;
        return true;
    }
}