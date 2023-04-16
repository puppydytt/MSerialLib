#include <iostream>
#include "MSerialLib/MSerialLib.h"

using namespace std;
using namespace MSerial;

void menu(MSerialLib &a) {
    int option = 0;
    cout << "enter message" << endl;
    char str[5000];
    scanf("%[^\n]s", str);
    while (true) {
        cout << "Enter the option which should be processed:\n"
                "1.\t Ping device with message\n"
                "2.\t Stop listener\n" <<
             "3.\t Quit program\n";
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
                a.stopListening(); // stop monitor
                break;
            case 3:
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

void callback(char * content) { //called when data is retrieved from I/O port
    cout << content << endl;
}

int main() {
    vector<string> existingSerialPorts;
    scan(existingSerialPorts); // scan for available ports
    for (auto &i: existingSerialPorts) {
        cout << i << endl;
    }

    MSerialLib a(existingSerialPorts[0]); // opens the first available serial port

    if (!a.openStream()) { // port ok?
        cerr << "Failed to open I/O stream" << endl;
    }

    a.setControl(115200, false, 1, 8);
    a.startListening(callback, 10);
    thread th1(menu, ref(a)); // run concurent thread with writeData in it
    th1.join();
    return a.closeStream();
}