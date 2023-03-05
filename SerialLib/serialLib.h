#ifndef SERIAL_LIB
#define SERIAL_LIB

#include <iostream>
#include <vector>
#include <windows.h>
#include <thread>
#include <chrono>
#include <mutex>

#define MAX_PORT 256

using namespace std;
using namespace chrono;

namespace Serial {

    class SerialLib {
    protected:

    private:
        HANDLE stream;
        _DCB controlBlock{};
        COMMTIMEOUTS timeouts{};
        char receivedByte;
        char sentByte;
        bool block;
        string iFaceName;

    public:
        SerialLib(string & iFaceName, int baudRate) : iFaceName(iFaceName), stream(nullptr), sentByte(0), receivedByte(0),
                                      block(false) {};

        bool openStream();

        void setControl(DWORD, BYTE, DWORD);

        bool receiveData(string &);

        //add spawn threads

        bool closeStream(const string &);

        bool serialStreamDataWrite(unsigned char *, uint64_t);

        void serialStreamReceiveData();
    };

}

#endif //SERIAL_LIB
