#include "serialLib.h"

namespace Serial {

    // refactor a little
    bool SerialLib::receiveData(string &dst, uint32_t timeout) {
        DWORD commModemStatus;
        char *buff(nullptr);
        DWORD bufferLength(0);
        DWORD bufferLLengthFromOverlapped(0);
        OVERLAPPED osReader = {0};
        COMSTAT portInfo = {0};
        DWORD nRead(0);

        memset(&osReader, 0, sizeof(osReader));

        osReader.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (!osReader.hEvent) {
            cerr << "ReceiveData Failed" << endl;
            return false;
        }

        if (!stream) {
            return false;
        }

        //EV_RXCHAR indicates that character was placed to input buffer
        SetCommMask(stream, EV_RXCHAR);
        WaitCommEvent(stream, &commModemStatus, &osReader);

        //suspend till EV_RXCHAR will fire
        switch (WaitForSingleObject(osReader.hEvent, timeout == 0 ? INFINITE : timeout)) {
            case WAIT_OBJECT_0:
                //Check is waiting for even completed successfully
                if (!GetOverlappedResult(stream, &osReader, &bufferLLengthFromOverlapped, false)) {
                    cerr << "WaitCommEventFailed errno " << GetLastError() << endl;
                    return false;
                }
                //retrieve info from comm port
                ClearCommError(stream, nullptr, &portInfo);
                bufferLength = portInfo.cbInQue;
                buff = new char[bufferLength];
                if (!ReadFile(stream, buff, bufferLength, &nRead, &osReader)) {
                    if (GetLastError() != ERROR_IO_PENDING)
                        cout << "Failed to read the stream errno " << GetLastError() << endl;
                }
                // suspend till ReadFile operation will complete its job
                switch (WaitForSingleObject(osReader.hEvent,
                                            200)) { //todo make as separate method
                    case WAIT_OBJECT_0:
                        if (!GetOverlappedResult(stream, &osReader, &bufferLLengthFromOverlapped, 200)) {
                            cerr << "WaitCommEventFailed errno " << GetLastError()
                                 << endl; //todo make it as a stringified value
                        } else {
                            cout << "nRead " << nRead << endl;
                            if (nRead > 0) {
                                dst = buff; // add \0 character to str to indicate ending of the string
                                //cout << buff << endl;
                            }
                        }
                        break;
                    case WAIT_TIMEOUT:
                        //process background info
                        cerr << "Failed to read buffer: TIMEOUT" << endl;
                        break;
                    default:
                        cerr << "Failed to read stream of COMM" << endl;
                }
                break;
            case WAIT_TIMEOUT:
                //process background info
                break;
            default:
                cerr << "Error occurred in comm event" << endl;
        }

        return true;
    }
}