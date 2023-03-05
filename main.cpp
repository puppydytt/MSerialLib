#include <iostream>
#include "SerialDriver/driver.h"


using namespace std;
using namespace Serial;

class Atmega328P : private SerialPHandler {
public:
    Atmega328P() : SerialPHandler() {};

    void SendInstruction() {
        unsigned char info[] = {'a', 'b', 'o', 'b', 'a', 's', 'u', 'c', 'k', 'm', 'y', 'd', 'i', 'c', 'k', 'p', 'l', 'e', 'a', 's', 'e'};
        serialStreamDataWrite(info, sizeof(info));
    }

    void EnableProgramming() {
        unsigned char info[4] = {'s', 'u', 'c', 'k'};
        checkExistingSerialPorts();
        if (existingSerialPorts.empty()) cerr << "No serial ports were found" << endl;
        else {
            for (const auto &port: existingSerialPorts) {
                if (openStream(port)) {
                    setControl(CBR_115200, NOPARITY, 1);
                    serialStreamDataWrite(info, sizeof(info));
                    thread abc([this] { this->serialStreamReceiveData(); });
                    abc.join();
                } else cerr << "Seems like PORT is occupied by other process " << port << endl;
            }
        }
    }

};

void menu(Atmega328P &a) {
    while (true) {
        int option = 0;
        cout << "Enter the option which should be processed:\n"
                "1.\t Ping device with message\n"
                "2.\t Program controlled device\n";
        cin >> option;
        switch (option) {
            case 1:
                cout << "fire";
                a.SendInstruction();
                break;
            case 2:
                break;
            default:
                cerr << "Provided not supported instruction\n" << endl;
        }
    }
}

int main() {
    Atmega328P c;
    thread th1(menu, ref(c));
    c.EnableProgramming();


    return 0;
}
