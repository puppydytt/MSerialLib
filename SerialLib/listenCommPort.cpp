#include "serialLib.h"

namespace Serial {
    bool SerialLib::startListening(callBack cb, uint32_t timeout) {
        if (cb == nullptr) return false;
        atomic_init(&listenerRunning, true);
        th = thread([this, cb, timeout] { this->spin(cb, timeout); });
        return true;
    }

    void SerialLib::spin(callBack cb, uint32_t timeout) {
        string ref;
        while (true) {
            if (!listenerRunning.load()) return;
            if (receiveData(ref, 0, timeout)) cb(const_cast<string &>(ref));
            ref.clear();
        }
    }

    /*
     * Setting event that signals receiveData to get out from INFINITE state
     * Terminates spin() thread
     * */

    bool SerialLib::stopListening() {
        if (!listenerRunning.load()) {
            cerr << "Listener is not running" << endl;
            return false;
        }
        bool signal = terminateReader();
        listenerRunning.store(!signal);
        return signal;
    }

    bool SerialLib::terminateReader() {
        {
            if (eventRead == nullptr) return false;
            return SetEvent(eventRead); // signal to the WaitForSingle object that even is signaled
        };
    }
}