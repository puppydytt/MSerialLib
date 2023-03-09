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
typedef void(callBack)(const string &);

namespace Serial {
    class SerialLib {

    private:
        HANDLE stream;
        _DCB controlBlock{};
        COMMTIMEOUTS timeouts{};
        string iFaceName;
        thread th;
        atomic<bool> listenerRunning;
        HANDLE eventRead;
        HANDLE eventWrite;


        void spin(callBack);

    public:
        SerialLib(string &iFaceName, int baudRate) : iFaceName(iFaceName), stream(nullptr) {};

        bool openStream();

        void setControl(DWORD, BYTE, DWORD);

        /**
         *
         * @param timeout(ms) if passed 0 applies INFINITE waiting for method,
         * @return
         */

        // don't use this along with startListening() when it's set to INFINITE mode, otherwise it will return immediately,as well as don't use it in concurrent threads. It will lead you to immediate return on one of them
        bool receiveData(string &, uint32_t);

        bool closeStream(const string &);

        bool terminateReader();

        bool write(const char * const, uint64_t);

        bool startListening(callBack);

        bool stopListening();

    };

}

#endif //SERIAL_LIB
