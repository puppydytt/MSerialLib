#include <iostream>
#include "MSerialLib/MSerialLib.h"

using namespace std;
using namespace MSerial;

void menu(MSerialLib &a) {
    int option = 0;
    char str[5000];
    scanf("%[^\n]s", str);
    while (true) {
        cout << "Enter the option which should be processed:\n"
                "1.\t Ping device with message\n"
                "2.\t Program controlled device\n" <<
             "3.\t Stop listener\n" <<
             "4.\t Quit program\n";
        if (!(cin >> option)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cerr << "Invalid input. Try again.\n";
        }
        switch (option) {
            case 1:
                cout << a.writeData(str, strlen(str)) << endl;
                break;
            case 2:
                break;
            case 3:
                a.stopListening();
                break;
            case 4:
                return;
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

void callback(const string &content) {
    cout << "msg from call back: " << content << endl;
}

int main() {
    vector<string> existingSerialPorts;
    scan(existingSerialPorts);

    for (auto &i: existingSerialPorts) {
        cout << i << endl;
    }
    MSerialLib a(existingSerialPorts[0]);

    if (!a.openStream()) {
        cerr << "Failed to open I/O stream" << endl;
    }
    a.setControl(300, false, 1, 8);

    a.startListening(callback, 1300);
    thread th1(menu, ref(a));
    string res;
//    a.receiveData(res, infinity, 1300);
//    cout << res << endl;
//    res.clear();
//    a.receiveData(res, infinity, 1300);
//    cout << res << endl;
    th1.join();
    return a.closeStream();
}

