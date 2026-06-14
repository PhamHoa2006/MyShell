/*
 * Exercise 06: File Mapping (Memory-mapped files)
 * Description: Demonstrates how to map a file into memory using CreateFileMapping 
 *              and MapViewOfFile. This allows reading/writing files as if they were 
 *              arrays in memory.
 *
 * Key APIs covered:
 * - CreateFileMapping: Creates or opens a named or unnamed file mapping object.
 * - MapViewOfFile: Maps a view of a file mapping into the address space of a calling process.
 * - UnmapViewOfFile: Unmaps a mapped view of a file from the calling process's address space.
 *
 * Tiếng Việt:
 * Bài tập 06: Ánh xạ tệp vào bộ nhớ (File Mapping)
 * Mô tả: Hướng dẫn cách ánh xạ một tệp tin vào bộ nhớ, cho phép thao tác trên tệp 
 *        như thao tác trên mảng bộ nhớ.
 */

#include <windows.h>
#include <iostream>
#include <string>

void printError(const char* msg) {
    std::cerr << msg << " Error code: " << GetLastError() << std::endl;
}

int main() {
    const char* filename = "mapped_file.txt";
    
    // Bước 1: Tạo một file mới / Step 1: Create a new file
    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printError("Failed to create file.");
        return 1;
    }

    // Viết một chút dữ liệu khởi tạo để file có kích thước / Write some initial data
    const char* initialData = "Hello, this is file mapping in Windows API!";
    DWORD bytesWritten;
    WriteFile(hFile, initialData, strlen(initialData), &bytesWritten, NULL);

    // Bước 2: Tạo đối tượng mapping file / Step 2: Create file mapping object
    // Yêu cầu quyền PAGE_READWRITE / Requires PAGE_READWRITE permission
    HANDLE hMapping = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READWRITE,
        0, 
        0, // Kích thước mapping (0 = toàn bộ file)
        NULL
    );

    if (hMapping == NULL) {
        printError("Failed to create file mapping.");
        CloseHandle(hFile);
        return 1;
    }

    // Bước 3: Ánh xạ file vào bộ nhớ / Step 3: Map view of file into memory
    char* mappedData = (char*)MapViewOfFile(
        hMapping,
        FILE_MAP_ALL_ACCESS, // Cấp quyền đọc/ghi
        0, 
        0,
        0 // Số byte ánh xạ (0 = toàn bộ mapping)
    );

    if (mappedData == NULL) {
        printError("Failed to map view of file.");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    // In ra dữ liệu trong memory (đến từ file) / Print data from memory
    std::cout << "Original data from mapped file: " << mappedData << std::endl;

    // Thay đổi dữ liệu trực tiếp qua con trỏ / Modify data directly via pointer
    std::cout << "Modifying mapped memory..." << std::endl;
    memcpy(mappedData, "HELLO", 5);

    // Đồng bộ hoá thay đổi xuống ổ cứng (tuỳ chọn) / Synchronize changes to disk (optional)
    FlushViewOfFile(mappedData, 0);

    std::cout << "Modified data: " << mappedData << std::endl;

    // Dọn dẹp / Cleanup
    UnmapViewOfFile(mappedData);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    std::cout << "File mapping closed successfully." << std::endl;
    return 0;
}
