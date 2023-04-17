#include "../MSerialLib.h"

namespace MSerial {
    bool MSerialLib::simpleRead(char **dst, uint32_t size) {
        DWORD nRead = 0;

        if (!stream) {
            return false;
        }

        *dst = new char[size];

        //Read data from I/O buffer, fill osReader with eventRead
        if (ReadFile(stream, *dst, size, &nRead, nullptr)) {
            return size == nRead;
        }

        // Failed to read byte from buffer
        return false;
    }
}