# MSerialLib

MSerialLib is a library written in C++ for communicating over serial interface

## Building the application with MSerialLib

Project is using CMake to build a projects

### *Requirements*

* Cmake version >= <span style="color:red; font-weight: bold">3.21.2</a><span>
* Compiler Which is using ThreadModel: <span style="color:red; font-weight: bold">posix</span>
* GCC version >= <span style="color:red; font-weight: bold">8.1.0</a><span>

### *Notes*

This build assumes that you have necessary compiler in your PATH, otherwise provide it with ```CMAKE_C_COMPILER```
and ```CMAKE_CXX_COMPILER```\
The ```GENPATH``` is path to your generator it can be MinGW Makefile and other type of Makefiles or Ninja toolset\
The ```Generator``` is the name of the generator for which was provided appropriate executable in ```GENPATH``` for
example ```MinGW``` or ```Ninja```

#### Generate CMake build files

* Debug

```bash
cd "path/to/project/dir" && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM="GENPATH" -G Generator -S ./ -B ./debug-build
```

* Release

```bash
cd "path/to/project/dir" && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="GENPATH" -G Generator -S ./ -B ./release-build
```

#### Build the program

* Debug

```bash
cd "./debug-build" && cmake --build ./ --target SerialMonitor
```

* Release

```bash
cd "./release-build" && cmake --build ./ --target SerialMonitor
```

## Usage
For more detailed example of using MSerialLib  checkout ```UsageExampleSerialMontor.cpp``` 
### Opens and sends data to the port

```c++
#include <iostream>
#include "MSerialLib/MSerialLib.h"

using namespace std;
using namespace MSerial;

int main(int argc, char** argv){
    string port = "COM4"; // name of the serial interface
    auto serial = MSerialLib(port);
    if (!serial.openStream()) { //open stream
        cerr << "Failed to open I/O stream" << endl;
    }

    serial.setControl(300, false, 1, 8); // 1 - baudRate, 2 - checkForParity, 3 - stopBits, 4 - byteSize(usually 8)

    string msg = "Hello from MSerialLib";
    //writData can be used independently, even in concurent thread with receiveData
    // but if already one writeData is running, another one can't be issued until first one will finish it's writing 
    serial.writeData(msg.c_str(), msg.length()); // 1 - pointer to msg, 2 - length of data which should be sent
    // suspended till write completes
    serial.closeStream() // when interface is closed, other programs can use it
    return 0;
}
```

### Opens and waits till data will appear on serial port

<span style="color: red">Warning don't use this function with startListening or with other <em>receiveData()</em> in concurrent threads</span>

```c++
#include <iostream>
#include "MSerialLib/MSerialLib.h"

using namespace std;
using namespace MSerial;

int main(int argc, char** argv) {
    string port = "COM4";
    auto serial = MSerialLib(port);
    if (!serial.openStream()) { //open stream
        cerr << "Failed to open I/O stream" << endl;
    }

    serial.setControl(300, false, 1, 8); // 1 - baudRate, 2 - checkForParity, 3 - stopBits, 4 - byteSize(usually 8)
    string rcv;
    //receives all data from serial port
    serial.receiveData(rcv, infinity, 1300); //1 - reference to buff where info will be stored, 2 - timeout(ms) how for receive event, 3 - read interval timeout
    //suspended till data will arrive
    cout << rcv << endl;
    serial.closeStream();
    return 0;
```
If the timeout is **infinity** or some long value, in other thread use
```c++
serial.terminateReader()
```
this will return on receiveData function, and current thread won't be suspended


--Imagine* if you want to receive really message with overall size of 200bytes from the device,
but you have speed of communication 300 bauds, which equals 37.5 bytes per second
when this value is not 0, MSerialLib suspends and issues new read in this timeout to read all input from buffer
This can effectively help you to receive absolutely whole message from your counterpart. But it can be also 
harmful... When your device sending data without any delay the data receiveData will be suspended forever till device won't
stop or delay between them will be more than provided(1300ms).

If you want it to work like arduino serial monitor use following
```c++
string rcv;
 while(1) {
        serial.receiveData(rcv, infinity, 0); //1 - reference to buff where info will be stored, 2 - timeout(ms) how for receive event, 3 - read interval timeout
        cout << rcv << endl;
    }
```
This will receive all data without any delay, but message would come partly.
### Listening communication port (monitor mode)
<span style="color: red"> Warning don't use this function along with receiveData in concurrent threads</span>

```c++
#include <iostream>
#include "MSerialLib/MSerialLib.h"

using namespace std;
using namespace MSerial;

// function which will be called when data is retrieved from I/O buffer
void callback(const string &msg) {
    cout << msg << endl;
}

int main(int argc, char** argv) {
    string port = "COM4";
    auto serial = MSerialLib(port);
    if (!serial.openStream()) { //open stream
        cerr << "Failed to open I/O stream" << endl;
    }

    serial.setControl(300, false, 1, 8); // 1 - baudRate, 2 - checkForParity, 3 - stopBits, 4 - byteSize(usually 8)

    serial.startListening(callback, 0); //args: 1- callback,2 - read interval timeout  listens port and when data is available call the callback and passing retrieved data there
    while(1) { //suspend program(to simulate thread.join())

    }
    return 0;
}
```

To stop the monitor in other thread use

```c++
serial.stopListening()
```




## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

[GNU General Public License v3.0](https://choosealicense.com/licenses/gpl-3.0/)