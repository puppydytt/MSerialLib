#ifndef SERIAL_LIB
#define SERIAL_LIB

#include <iostream>
#include <vector>
#include <windows.h>
#include <thread>
#include <atomic>


#define MAX_PORT 256

using namespace std;
using namespace chrono;

// add param namings
typedef void(callBack)(const string&);

namespace Serial {
    class SerialLib {

    protected:
        void spin(callBack);

    private:
        HANDLE stream;
        _DCB controlBlock{};
        COMMTIMEOUTS timeouts{};
        string iFaceName;
        thread th;
        atomic<bool> listenerRunning;
        OVERLAPPED cpy;
    public:
        SerialLib(string &iFaceName, int baudRate) : iFaceName(iFaceName), stream(nullptr) {};

        bool openStream();

        void setControl(DWORD, BYTE, DWORD);

        /**
         *
         * @param timeout(ms) if passed 0 applies INFINITE waiting for method,
         * @return
         */

        bool receiveData(string &, uint32_t);

        //add serial port listener based on the recieve data function, make spining thread which shared mutexed area with signals, add logic for thread stop

        bool closeStream(const string &);

        bool write(unsigned char *, uint64_t);

        bool startListening(callBack);

        bool stopListening();

    };

}

#endif //SERIAL_LIB
