/*
 * Exercise 07: Asynchronous File I/O
 * Description: Demonstrates non-blocking file reads using the OVERLAPPED structure.
 *              The program initiates a read and continues executing other tasks while 
 *              waiting for the I/O to complete.
 *
 * Key APIs covered:
 * - CreateFile: Opened with FILE_FLAG_OVERLAPPED flag.
 * - ReadFile: Used with an OVERLAPPED structure to perform async read.
 * - GetOverlappedResult: Checks the result of an asynchronous operation.
 * - WaitForSingleObject: Waits for the event inside OVERLAPPED to be signaled.
 *
 * Tiếng Việt:
 * Bài tập 07: Thao tác tệp bất đồng bộ (Async I/O)
 * Mô tả: Minh hoạ cách đọc file không chặn luồng chính (non-blocking) dùng OVERLAPPED.
 */

#include <windows.h>
#include <iostream>
#include <string>

void printError(const char* msg) {
    std::cerr << msg << " Error code: " << GetLastError() << std::endl;
}

int main() {
    const char* filename = "async_test.txt";

    // Tạo file mẫu trước / Create a sample file first
    HANDLE hFileSetup = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFileSetup != INVALID_HANDLE_VALUE) {
        const char* text = "This is a large text file for async I/O testing. Welcome to Windows API programming!";
        DWORD bw;
        WriteFile(hFileSetup, text, strlen(text), &bw, NULL);
        CloseHandle(hFileSetup);
    }

    // Bước 1: Mở file với cờ FILE_FLAG_OVERLAPPED / Step 1: Open file with OVERLAPPED flag
    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // QUAN TRỌNG: Cờ bất đồng bộ
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printError("Failed to open file.");
        return 1;
    }

    // Bước 2: Chuẩn bị cấu trúc OVERLAPPED / Step 2: Prepare OVERLAPPED structure
    OVERLAPPED overlapped = {0};
    overlapped.Offset = 0; // Đọc từ đầu file
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Sự kiện để theo dõi tiến độ

    if (overlapped.hEvent == NULL) {
        printError("Failed to create event.");
        CloseHandle(hFile);
        return 1;
    }

    // Bước 3: Bắt đầu đọc bất đồng bộ / Step 3: Start async read
    char buffer[100] = {0};
    DWORD bytesRead = 0;

    std::cout << "Initiating asynchronous read..." << std::endl;
    BOOL bResult = ReadFile(
        hFile,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        &overlapped
    );

    // Xử lý kết quả trả về của ReadFile / Handle ReadFile result
    if (bResult == FALSE) {
        DWORD error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            std::cout << "I/O is pending. Doing some other work in the meantime..." << std::endl;
            
            // Làm việc khác trong khi chờ đọc file / Do something else while reading
            for (int i = 0; i < 3; ++i) {
                std::cout << "Working... " << i + 1 << std::endl;
                Sleep(500); // Giả lập công việc / Simulate work
            }

            // Chờ cho đến khi I/O hoàn tất / Wait until I/O completes
            std::cout << "Waiting for I/O completion..." << std::endl;
            WaitForSingleObject(overlapped.hEvent, INFINITE);

            // Lấy kết quả / Get the result
            bResult = GetOverlappedResult(hFile, &overlapped, &bytesRead, FALSE);
            if (bResult) {
                std::cout << "Async read completed! Bytes read: " << bytesRead << std::endl;
                std::cout << "Data: " << buffer << std::endl;
            } else {
                printError("GetOverlappedResult failed.");
            }
        } else {
            printError("ReadFile failed immediately.");
        }
    } else {
        // I/O hoàn tất ngay lập tức (thường do file đã nằm trong cache hệ thống)
        // I/O completed synchronously (often because file is in system cache)
        std::cout << "Read completed synchronously. Bytes read: " << bytesRead << std::endl;
        std::cout << "Data: " << buffer << std::endl;
    }

    // Dọn dẹp / Cleanup
    CloseHandle(overlapped.hEvent);
    CloseHandle(hFile);
    
    // Xoá file / Delete the sample file
    DeleteFileA(filename);

    return 0;
}
