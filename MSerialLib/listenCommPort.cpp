#include "MSerialLib.h"

namespace MSerial {
    bool MSerialLib::startListening(CallBack cb, uint32_t timeout) {
        if (cb == nullptr) return false;
        atomic_init(&listenerRunning, true);
        th = thread([this, cb, timeout] { this->spin(cb, timeout); });
        return true;
    }

    void MSerialLib::spin(CallBack callback, uint32_t timeout) {
        char * buff;
        while (true) {
            if (!listenerRunning.load()) return;
            if (receiveData(&buff, infinity, timeout)) {
                callback(buff);
            }
            memset(buff, 0, strlen(buff));
        }
    }

    /*
     * Setting event that signals receiveData to get out from INFINITE state
     * Terminates spin() thread
     * */

    bool MSerialLib::stopListening() {
        if (!listenerRunning.load()) {
            cerr << "Listener is not running" << endl;
            return false;
        }
        bool signal = terminateReader();
        listenerRunning.store(!signal);
        return signal;
    }

    bool MSerialLib::terminateReader() {
        if (eventRead == nullptr) return false;
        return SetEvent(eventRead); // signal to the WaitForSingle object that even is signaled
    }
}