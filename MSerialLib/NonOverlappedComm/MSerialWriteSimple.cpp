
#include "../MSerialLib.h"

namespace MSerial {
    uint32_t MSerialLib::simpleWrite(char * buff, uint32_t len) {
        DWORD written = 0;

        if (!stream) {
            return false;
        }

        if(WriteFile(stream, buff, len, &written, nullptr)) {
            return len == written;
        } else return false;
    }
}