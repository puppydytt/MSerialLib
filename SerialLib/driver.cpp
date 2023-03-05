#include "driver.h"

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

    bool SerialLib::serialStreamDataWrite(unsigned char *data, uint64_t length) {
        DWORD written;
        OVERLAPPED osWrite = {0};

        osWrite.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (!osWrite.hEvent) {
            cerr << "Event creation failed" << endl;
            return false;
        }

        osWrite.Internal = 0;
        osWrite.InternalHigh = 0;
        osWrite.Offset = 0;
        osWrite.OffsetHigh = 0;

        if (stream) { // create separate method for stream check
            if (!WriteFile(stream, data, length, &written, &osWrite)) {
                if (GetLastError() != ERROR_IO_PENDING) {
                    //write is not pending and failed
                    cerr << "Error occurred in I/0" << endl;
                    return false;
                } else {
                    switch (WaitForSingleObject(osWrite.hEvent, INFINITE)) {
                        case WAIT_OBJECT_0:
                            if (!GetOverlappedResult(stream, &osWrite, &written, FALSE));
                            else return written;
                            break;
                        default:
                            break;
                    }
                }
            }
            return written;
        } else return false;
    }

    void SerialLib::serialStreamReceiveData() {
        DWORD commModemStatus;
        char *buff(nullptr);
        DWORD bufferLength(0);
        DWORD bufferLLengthFromOverlapped(0);
        OVERLAPPED osReader = {0};
        COMSTAT portInfo = {0};
        DWORD nRead(0);
        osReader.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (!osReader.hEvent) {
            cerr << "RX: Event creation failed" << endl;
            return;
        }

        osReader.InternalHigh = 0;
        osReader.Internal = 0;
        osReader.Offset = 0;
        osReader.OffsetHigh = 0;

        if (stream) {
            while (true) {
                SetCommMask(stream, EV_RXCHAR);
                WaitCommEvent(stream, &commModemStatus, &osReader);
                switch (WaitForSingleObject(osReader.hEvent, 200)) {
                    case WAIT_OBJECT_0:
                        if (!GetOverlappedResult(stream, &osReader, &bufferLLengthFromOverlapped, 200)) {
                            cerr << "WaitCommEventFailed errno " << GetLastError()
                                 << endl; //todo make it as a stringified value
                        } else {
                            ClearCommError(stream, nullptr, &portInfo);
                            bufferLength = portInfo.cbInQue;
                            buff = new char[bufferLength];
                            if (!ReadFile(stream, buff, bufferLength, &nRead, &osReader)) {
                                if (GetLastError() != ERROR_IO_PENDING)
                                    cout << "Failed to read the stream errno " << GetLastError() << endl;
                            }
                            switch (WaitForSingleObject(osReader.hEvent,
                                                        50)) { //todo make as separate method of separate class
                                case WAIT_OBJECT_0:
                                    if (!GetOverlappedResult(stream, &osReader, &bufferLLengthFromOverlapped, 200)) {
                                        cerr << "WaitCommEventFailed errno " << GetLastError()
                                             << endl; //todo make it as a stringified value
                                    } else {
                                        cout << "nRead " << nRead << endl;
                                        if (nRead > 0) {
                                            cout << buff << endl;
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
                        }
                        break;
                    case WAIT_TIMEOUT:
                        //process background info
                        break;
                    default:
                        cerr << "Error occurred in comm event" << endl;
                }
            }
            cerr << "COM port is not specified" << endl;
        }
        //todo call reinit after receive cycle

    }

    void SerialLib::setControl(DWORD baud, BYTE parity, DWORD timeout) {
        if (!stream) return;
        GetCommState(stream, &controlBlock);
        controlBlock.BaudRate = baud;
        controlBlock.fParity = parity;
        controlBlock.StopBits = 1;
        controlBlock.ByteSize = 8;
        SetCommState(stream, &controlBlock);

        GetCommTimeouts(stream, &timeouts);
        timeouts.ReadIntervalTimeout = timeout;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 0;
        SetCommTimeouts(stream, &timeouts);
    }
}
