#include <iostream>
#include "SerialLib/serialLib.h"


using namespace std;
using namespace Serial;

// a = ForwardCommunication({commParameters}) #  apply structure to port
// a.sendMessage(msg,msg_size) # send message
// a.listenForNewMessages(listener) # run the thread receiving component
// a.detectAvailableDevices()
// a.forwardClose() depreciates connection


/*
class Atmega328P : private SerialLib {
public:
    Atmega328P() : SerialLib() {};

    void SendInstruction() {
        unsigned char info[] = {'a', 'b', 'o', 'b', 'a', 's', 'u', 'c', 'k', 'm', 'y', 'd', 'i', 'c', 'k', 'p', 'l',
                                'e', 'a', 's', 'e'};
        serialStreamDataWrite(info, sizeof(info));
    }

    void EnableProgramming() {
        unsigned char info[4] = {'s', 'u', 'c', 'k'};
        scan();
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


*/

void menu(SerialLib &a) {
    while (true) {
        int option = 0;
        char str[5000];
        scanf("%[^\n]s", str);
        cout << "Enter the option which should be processed:\n"
                "1.\t Ping device with message\n"
                "2.\t Program controlled device\n" <<
             "3.\t Kill the thread\n";
        if (!(cin >> option)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cerr << "Invalid input. Try again.\n";
        }
        switch (option) {
            case 1:
                cout << a.write(str, strlen(str)) << endl;
                break;
            case 2:
                break;
            case 3:
                a.stopListening();
                break;
            default:
                cerr << "Provided not supported instruction\n" << endl;
                continue;
        }
    }
}

void scan(vector<string> &a) {
    char lpBuff[5000];
    string COM = "COM";
    for (int i = 1; i < MAX_PORT; i++) {
        string port = COM + to_string(i);
        if (QueryDosDevice(port.c_str(), lpBuff, 5000) != 0) a.push_back(port);
    }
}

// documenting the library, describe why synchronous-overlapped was selected instead of asynchronous

void cb(const string &content) {
    cout << "msg from call back: " << content << endl;
}

int main() {
    vector<string> existingSerialPorts;
    scan(existingSerialPorts);

    for (auto &i: existingSerialPorts) {
        cout << i << endl;
    }
    SerialLib a(existingSerialPorts[0], 9600);

    if (!a.openStream()) {
        cerr << "Failed to open I/O stream" << endl;
    }
    a.setControl(300, false, 200);

    a.startListening(cb, 800);
    string res;

    thread th1(menu, ref(a));
//    a.receiveData(res, 0);
   // cout << "msg: " << res << endl;
    th1.join();


    //thread th1(menu, ref(c));
    //c.EnableProgramming();

    return 0;
}

