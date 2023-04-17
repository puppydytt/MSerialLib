#ifndef SERIAL_LIB
#define SERIAL_LIB

#include <iostream>
#include <vector>
#include <windows.h>
#include <thread>
#include <atomic>

#define MAX_PORT 256
#define infinity INFINITE

using namespace std;
using namespace chrono;

typedef void(CallBack)(char *);

namespace MSerial {
    class MSerialLib {

    private:
        /**pointer to serial port*/
        HANDLE stream{};
        /**configuration of serialPort */
        _DCB controlBlock{};
        /**serial port name*/
        string iFaceName;
        /**spin thread*/
        thread th;
        /**atomic bool for handling stop signal of spin thread*/
        atomic<bool> listenerRunning{};
        /**pointer to read thread*/
        HANDLE eventRead{};
        /**pointer to write thread*/
        HANDLE eventWrite{};

        /**part of serial port listener*/
        void spin(CallBack cb, uint32_t timeout);

    public:
        explicit MSerialLib(string &iFaceName) : iFaceName(iFaceName) {};

        /**
         * @name openStream()
         * @brief Creates hand to serial port
         * @param bool state - if true overlapped enabled, false disabled
         * @returns
         * true - if serial port was opened
         * false - failure(port is busy?)
         */

        bool openStream(bool);

        /**
         * @name receiveData()
         * @brief
         * If called suspends current thread in which it was called and retrieves all characters which came to input buffer
         * @param buff pointer to (char *) where retrieved content will be placed
         * @param timeout (ms) time for which function listens if there is upcoming content on serial port, the value passed should be is ms(milliseconds)
         * but function also supports macro 'infinity' which will wait until action happens
         * @param readBufferTimeout (ms) timeout which will be applied between read intervals(depending on your communication), when '0' is passed timeout is disabled\n
         * usually to retried message without this timeout is quite short it can be received in while loop for example to get complete image, but much easier to use timeout
         * for long messages for example if speed 300 baud use timeout 1300ms to retrieve full message from device
         * @warning
         * Only one read operation is permitted on stream and it should be completed before other read will be issued.
         * Don't use this along with startListening(), otherwise it will return immediately,as well as don't use it in concurrent threads.
         * It will lead to immediate function error return on one of them
         * @returns
         * true - succeeded\n
         * false - failed
         **/

        bool receiveData(char **, uint32_t timeout, uint32_t readBufferTimeout);



        /**
          * @name simpleRead()
          * @brief
          * Reading everything from serial buffer NON OVERLAPPED
          * @param char ** dst, pointer to char * where the read buffer will be assigned
          * @param int n, how many characters should be read from the buffer
          * @return true if succeeds false if fails
          */
         bool simpleRead(char **, uint32_t);

        /**
         * @name simpleWrite()
         * @brief
         * Writing to serial port from buffer NON OVERLAPPED
         * @param char * buff, pointer to buffer
         * @param int n, how many bytes should be written
         * @return true if succeeds false if fails
         */

        uint32_t simpleWrite(char *, uint32_t);

        /**
        * @name writeData()
        * @brief
        * If called suspends current thread in which it was called and sends characters to serial port
        * @param data pointer to data which you need to send
        * @param length length of data which should be transmitted
        * @warning
        * Only one write operation is permitted on stream and it should be completed before other write will issued.
        * Don't use it in concurrent threads, or if you are using check if calling is synchronous
        * otherwise will lead to immediate function error return on one of them.
        * @return
        * true - succeeded\n
        * false - failed
        **/
        bool writeData(const char *data, uint64_t length);

        /**
       * @name setControl()
       * @brief
       * Applies configuration to serial port
       * @param baudRate speed of communication with counter-part
       * @param parity checks for parity and report error
       * @param stopBits how many stop bits your TX transaction carries, used to indicate the end of TX transaction, usually it's 1 or 2
       * @param byteSize the number of bits in the bytes transmitted and received common value is 8
       **/
        void setControl(uint32_t baudRate, uint8_t parity, uint8_t stopBits, uint8_t byteSize);

        /**
         * @name closeStream()
         * @brief Closes the stream and makes other program to access it
         * @return:
         * true -  succeeded\n
         * false - failed
         **/
        bool closeStream();

        /**
         * @brief Terminates function receiveData. Especially useful when you want to kill receiveData if 'infinity' or long value was provided as timeout parameter
         * @name terminateReader()
         * @return
         * true - succeeded\n
         * false - failed
         **/
        bool terminateReader();

        /**
         * @name startListening()
         * @brief runs serial port listener in the background when data comes it calls a callback and gives there message which was received from I/O buffer
         * @param callBack callback which will be executed when message will be received
         * @param timeout timeout between read intervals, checkout readBufferTimeout on receiveData()
         * @warning
         * Don't use this function if receiveEvent which was ran in parallel thread, use it if you are sure that no read operation is going on on stream, otherwise it will return immediately
         * @returns
         * true - listener started successfully\n
         * false - filed to start listener
         */
        bool startListening(CallBack callBack, uint32_t timeout);

        /**
       * @name stopListening()
       * @brief stopsListening on serialPort, when stopped receiveBuffer can be called
       * @returns
       * true -  successfully stopped listener\n
       * false - filed to stop listener
       */
        bool stopListening();
    };
}

#endif //SERIAL_LIB
