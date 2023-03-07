#include "serialLib.h"

namespace Serial {
    bool SerialLib::startListening(callBack cb) {
        if (cb == nullptr) return false;
        atomic_init(&listenerRunning, true);
        th = thread([this, cb] { this->spin(cb); });
        return true;
    }

    void SerialLib::spin(callBack cb) {
        string ref;
        while (true) {
            cout << "a" << endl;
            if (!listenerRunning.load()) return;
            if (receiveData(ref, 0)) cb(const_cast<string &>(ref));
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
        bool signal =  SetEvent(cpy.hEvent); // add separate method for terminating the wait on the read event:)
        listenerRunning.store(!signal);
        return !signal;
    }
}