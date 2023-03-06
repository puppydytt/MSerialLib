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

// add param namings



namespace Serial {
    enum signalState {
        RUN = 0,
        STOP,
    };

    class SerialLib {

    protected:
        void spin();

    private:
        HANDLE stream;
        _DCB controlBlock{};
        COMMTIMEOUTS timeouts{};
        char receivedByte;
        char sentByte;
        bool block;
        string iFaceName;
        thread th;


    public:
        mutex m;
        signalState signal;
        SerialLib(string &iFaceName, int baudRate) : iFaceName(iFaceName), stream(nullptr), sentByte(0),
                                                     receivedByte(0),
                                                     block(false) {};

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

        void startListening();

        bool stopListening();

    };

}

#endif //SERIAL_LIB
