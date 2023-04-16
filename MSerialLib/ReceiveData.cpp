#include "MSerialLib.h"

namespace MSerial {
    bool MSerialLib::receiveData(char ** dst , uint32_t timeout, uint32_t fillBufferTimeout) {
        DWORD commModemStatus;
        char *buff(nullptr);
        DWORD bufferLength(1); // +1 reservation for '\0' character
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
                //Retrieve info from comm port (bytes in input buffer + bytes in output buffer)
                while (true) {
                    //thread sync, wait for write thread to complete its own operation
                    WaitForSingleObject(eventWrite, INFINITE);
                    ClearCommError(stream, nullptr, &portInfo);
                    bufferLength += portInfo.cbInQue;
                    char *tmpBuff = static_cast<char *>(realloc(buff,
                                                                bufferLength));
                    if (!tmpBuff) {
                        cerr << "Failed to reallocate buff: ReceiveData" << endl;
                        return false;
                    }
                    buff = tmpBuff;

                    //Read data from I/O buffer, fill osReader with eventRead
                    if (!ReadFile(stream, &buff[offset] /*offset*/, portInfo.cbInQue, &nRead,
                                  &overlappedReadEvent)) {
                        if (GetLastError() != ERROR_IO_PENDING) {
                            cout << "Failed to read the stream errno " << GetLastError() << endl;
                        }
                    }
                    // suspend till ReadFile operation will complete its job
                    if (WaitForSingleObject(overlappedReadEvent.hEvent, INFINITE) == WAIT_OBJECT_0) {
                        if (!GetOverlappedResult(stream, &overlappedReadEvent, &bufferLLengthFromOverlapped,
                                                 INFINITE)) {
                            cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                        } else {
                            //OK ReadFile done without any errors
                            if (nRead == 0) {
                                // all data was received successfully
                                break;
                            }
                        }
                    } else {
                        cerr << "Failed to read I/O buffer" << endl;
                        return false;
                    }
                    offset += portInfo.cbInQue;
                    if (fillBufferTimeout > 0) Sleep(fillBufferTimeout);
                }
                --bufferLength; // compensate '/0' character on buffer
                if (bufferLength != 0) {
                    buff[bufferLength] = '\0';
                    *dst = buff;
                    //required call, prevents call of WaitForSingle object with empty buffer
                    ResetEvent(overlappedRXEvent.hEvent);
                    WaitCommEvent(stream, &commModemStatus, &overlappedRXEvent);
                    WaitForSingleObject(overlappedRXEvent.hEvent, 0);
                    eventRead = nullptr;
                    return true;
                }
                break;
            case WAIT_TIMEOUT:
                //process background info
                return false;
            default:
                cerr << "Error occurred in comm event" << endl;
                return false;
        }
        return false;
    }
}