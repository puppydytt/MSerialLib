#include "serialLib.h"

namespace Serial {
    bool SerialLib::write(unsigned char *data, uint64_t length) {
        DWORD messageLength;
        OVERLAPPED osWrite;

        // initializing overlapped structure
        memset(&osWrite, 0, sizeof(osWrite));

        osWrite.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (!osWrite.hEvent) {
            cerr << "Event creation failed" << endl;
            return false;
        }

        if (!stream) return false;

        // sending the data to the low-level component, creating setting overlapped event to osWrite structure
        // remark using nullptr for lpNumberOfBytesWritten because operation is overlapped, refer to https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
        if (!WriteFile(stream, data, length, nullptr, &osWrite)) {
            // WriteFile in overlapped returns immediately non-zero value, checking the indicator for error
            if (GetLastError() != ERROR_IO_PENDING) {  // ERROR_IO_PENDING = OK, operation still I/O req pending
                cerr << "Write operation on I/O failed";
                return false;
            }
        }

        //thread suspension waiting for WriteFile overlapped event to finish its job
        switch (WaitForSingleObject(osWrite.hEvent, 200)) {
            case WAIT_OBJECT_0:
                //Gathering info on completed event
                if(!GetOverlappedResult(stream, &osWrite, &messageLength, false)) {
                    cerr << "Failed while retrieving overlapped results" << endl;
                    return false;
                }
                break;
            case WAIT_TIMEOUT:
                cerr << "Write operation on I/O failed: Time out" << endl;
                return false;
            default:
                break;
        }
        return length == messageLength;
    }
}