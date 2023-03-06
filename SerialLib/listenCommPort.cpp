#include "serialLib.h"

namespace Serial {
    void SerialLib::startListening() {
        signal = RUN;
        th = thread([this] { this->spin(); });
    }

    void SerialLib::spin() {
        string ref;
        while (true) {

            if (signal == STOP) {
                cout << "stop" << endl;
                return;
            }

            if (receiveData(ref, 200)) {
                cout << ref << endl;
            };
        }
    }

    bool SerialLib::stopListening() {
        if (signal == STOP) {
            cerr << "listener is not running" << endl;
            return false;
        } else signal = STOP;
        return true;
    }
}