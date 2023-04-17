#include "MSerialLib.h"

namespace MSerial {
    bool MSerialLib::openStream(bool isOverlapped) {
        stream = CreateFile(iFaceName.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,      //  must be opened with exclusive-access
                            nullptr,   //  default security attributes
                            OPEN_EXISTING, //  must use OPEN_EXISTING
                            isOverlapped ? FILE_FLAG_OVERLAPPED : 0,      //overlapped I/O
                            nullptr); //  hTemplate must be NULL for comm devices
        return stream != INVALID_HANDLE_VALUE;
    }


    bool MSerialLib::closeStream() {
        return CloseHandle(stream);
    }

    void MSerialLib::setControl(uint32_t baudRate, uint8_t parity, uint8_t stopBits, uint8_t byteSize) {
        if (!stream) return;
        GetCommState(stream, &controlBlock);
        controlBlock.BaudRate = baudRate;
        controlBlock.fParity = parity;
        controlBlock.StopBits = stopBits;
        controlBlock.ByteSize = byteSize;
        SetCommState(stream, &controlBlock);
    }
}
