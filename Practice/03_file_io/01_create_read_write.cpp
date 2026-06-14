/*
 * ============================================================================
 * BÀI TẬP 01: TẠO, GHI VÀ ĐỌC FILE (Create, Write & Read Files)
 * ============================================================================
 * Mô tả / Description:
 *   Bài tập này minh họa các thao tác cơ bản nhất với file trên Windows:
 *   tạo file mới, ghi dữ liệu vào file, và đọc dữ liệu từ file.
 *
 *   This exercise demonstrates the most fundamental file operations on Windows:
 *   creating a new file, writing data to it, and reading data back.
 *
 * Các API chính / Key APIs:
 *   - CreateFile()   : Tạo hoặc mở file / Create or open a file
 *   - WriteFile()    : Ghi dữ liệu vào file / Write data to a file
 *   - ReadFile()     : Đọc dữ liệu từ file / Read data from a file
 *   - CloseHandle()  : Đóng handle file / Close a file handle
 *   - GetLastError() : Lấy mã lỗi cuối cùng / Get last error code
 *
 * Biên dịch / Compile:
 *   g++ 01_create_read_write.cpp -o 01_create_read_write.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Hàm tiện ích: In mã lỗi Windows kèm thông báo
// Utility: Print Windows error code with message
// ============================================================================
void PrintError(const char* operation) {
    DWORD err = GetLastError();
    char msgBuf[512];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
        msgBuf, sizeof(msgBuf), NULL
    );
    printf("[LOI/ERROR] %s that bai - Ma loi / Error code: %lu\n", operation, err);
    printf("  Chi tiet / Details: %s\n", msgBuf);
}

// ============================================================================
// PHẦN 1: Tạo và ghi file
// PART 1: Create and write to a file
// ============================================================================
BOOL CreateAndWriteFile(const char* filename) {
    printf("=== PHAN 1: TAO VA GHI FILE / CREATE AND WRITE FILE ===\n\n");

    /*
     * CreateFile() - API trung tâm để mở/tạo file trên Windows
     * CreateFile() - The central API for opening/creating files on Windows
     *
     * Tham số / Parameters:
     *   lpFileName       : Tên file / File name
     *   dwDesiredAccess  : Quyền truy cập - GENERIC_WRITE để ghi
     *                      Access rights - GENERIC_WRITE for writing
     *   dwShareMode      : Chế độ chia sẻ - 0 nghĩa là không chia sẻ (exclusive)
     *                      Share mode - 0 means no sharing (exclusive access)
     *   lpSecurityAttributes : Thuộc tính bảo mật - NULL dùng mặc định
     *                          Security attributes - NULL for default
     *   dwCreationDisposition : Cách tạo file
     *                           - CREATE_ALWAYS: Luôn tạo mới (ghi đè nếu tồn tại)
     *                           - CREATE_NEW: Chỉ tạo mới (lỗi nếu đã tồn tại)
     *                           - OPEN_EXISTING: Chỉ mở file đã có
     *                           - OPEN_ALWAYS: Mở nếu có, tạo nếu chưa có
     *                           - TRUNCATE_EXISTING: Mở và xóa nội dung
     *   dwFlagsAndAttributes : Thuộc tính file - FILE_ATTRIBUTE_NORMAL là bình thường
     *                          File attributes - FILE_ATTRIBUTE_NORMAL for normal files
     *   hTemplateFile    : File mẫu - thường là NULL
     *                      Template file - usually NULL
     */
    HANDLE hFile = CreateFileA(
        filename,                   // Tên file / File name
        GENERIC_WRITE,              // Quyền ghi / Write access
        0,                          // Không chia sẻ / No sharing
        NULL,                       // Bảo mật mặc định / Default security
        CREATE_ALWAYS,              // Luôn tạo mới / Always create new
        FILE_ATTRIBUTE_NORMAL,      // File bình thường / Normal file
        NULL                        // Không dùng template / No template
    );

    /*
     * Kiểm tra handle hợp lệ / Check for valid handle
     * INVALID_HANDLE_VALUE (-1) nghĩa là thao tác thất bại
     * INVALID_HANDLE_VALUE (-1) means the operation failed
     */
    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateFile (ghi/write)");
        return FALSE;
    }

    printf("[OK] Da tao file: %s\n", filename);
    printf("     File created successfully: %s\n\n", filename);

    // --- Ghi nhiều dòng vào file / Write multiple lines to the file ---
    const char* lines[] = {
        "Dong 1: Xin chao tu Windows File I/O!\r\n",
        "Line 2: Hello from Windows File I/O!\r\n",
        "Dong 3: Day la bai tap ve CreateFile, WriteFile, ReadFile.\r\n",
        "Line 4: This is an exercise about CreateFile, WriteFile, ReadFile.\r\n",
        "Dong 5: Ket thuc file.\r\n"
    };
    int lineCount = sizeof(lines) / sizeof(lines[0]);

    printf("[INFO] Dang ghi %d dong vao file...\n", lineCount);
    printf("       Writing %d lines to file...\n\n", lineCount);

    for (int i = 0; i < lineCount; i++) {
        DWORD bytesWritten = 0;
        DWORD bytesToWrite = (DWORD)strlen(lines[i]);

        /*
         * WriteFile() - Ghi dữ liệu vào file
         * WriteFile() - Write data to a file
         *
         * Tham số / Parameters:
         *   hFile             : Handle file đã mở / Open file handle
         *   lpBuffer          : Con trỏ tới dữ liệu cần ghi / Pointer to data
         *   nNumberOfBytesToWrite : Số byte cần ghi / Number of bytes to write
         *   lpNumberOfBytesWritten : [out] Số byte đã ghi thực tế
         *                            [out] Actual bytes written
         *   lpOverlapped      : NULL cho I/O đồng bộ / NULL for synchronous I/O
         *
         * Trả về TRUE nếu thành công / Returns TRUE on success
         */
        BOOL success = WriteFile(
            hFile,              // Handle file
            lines[i],           // Dữ liệu cần ghi / Data to write
            bytesToWrite,       // Số byte / Byte count
            &bytesWritten,      // Số byte đã ghi / Bytes written
            NULL                // I/O đồng bộ / Synchronous I/O
        );

        if (!success) {
            PrintError("WriteFile");
            CloseHandle(hFile);
            return FALSE;
        }

        printf("  Dong/Line %d: Da ghi/Wrote %lu/%lu bytes\n",
               i + 1, bytesWritten, bytesToWrite);
    }

    /*
     * CloseHandle() - Đóng handle file
     * CloseHandle() - Close the file handle
     *
     * QUAN TRỌNG: Luôn đóng handle sau khi sử dụng xong để giải phóng tài nguyên
     * IMPORTANT: Always close handles when done to release system resources
     * Quên đóng handle sẽ gây rò rỉ tài nguyên (resource leak)
     * Forgetting to close handles causes resource leaks
     */
    CloseHandle(hFile);
    printf("\n[OK] Da dong file sau khi ghi.\n");
    printf("     File closed after writing.\n\n");

    return TRUE;
}

// ============================================================================
// PHẦN 2: Đọc file
// PART 2: Read from a file
// ============================================================================
BOOL ReadFileContents(const char* filename) {
    printf("=== PHAN 2: DOC FILE / READ FILE ===\n\n");

    /*
     * Mở file để đọc / Open file for reading
     * Lưu ý: GENERIC_READ và OPEN_EXISTING
     * Note: GENERIC_READ and OPEN_EXISTING
     */
    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ,               // Quyền đọc / Read access
        FILE_SHARE_READ,            // Cho phép đọc đồng thời / Allow shared reading
        NULL,
        OPEN_EXISTING,              // File phải tồn tại / File must exist
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateFile (doc/read)");
        return FALSE;
    }

    printf("[OK] Da mo file de doc: %s\n\n", filename);

    /*
     * Lấy kích thước file trước khi đọc / Get file size before reading
     * GetFileSize() trả về kích thước file tính bằng byte
     * GetFileSize() returns the file size in bytes
     */
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        PrintError("GetFileSize");
        CloseHandle(hFile);
        return FALSE;
    }
    printf("[INFO] Kich thuoc file / File size: %lu bytes\n\n", fileSize);

    // Cấp phát bộ nhớ để đọc toàn bộ file / Allocate memory for entire file
    char* buffer = new char[fileSize + 1];
    if (!buffer) {
        printf("[LOI/ERROR] Khong du bo nho / Out of memory!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    DWORD bytesRead = 0;

    /*
     * ReadFile() - Đọc dữ liệu từ file
     * ReadFile() - Read data from a file
     *
     * Tham số / Parameters:
     *   hFile               : Handle file / File handle
     *   lpBuffer            : Bộ đệm nhận dữ liệu / Buffer to receive data
     *   nNumberOfBytesToRead : Số byte muốn đọc / Bytes to read
     *   lpNumberOfBytesRead  : [out] Số byte đã đọc thực tế / Actual bytes read
     *   lpOverlapped        : NULL cho I/O đồng bộ / NULL for synchronous I/O
     *
     * Trả về TRUE nếu thành công / Returns TRUE on success
     * Khi đọc đến cuối file (EOF), bytesRead = 0
     * When reaching EOF, bytesRead = 0
     */
    BOOL success = ReadFile(
        hFile,
        buffer,
        fileSize,
        &bytesRead,
        NULL
    );

    if (!success) {
        PrintError("ReadFile");
        delete[] buffer;
        CloseHandle(hFile);
        return FALSE;
    }

    // Kết thúc chuỗi bằng null / Null-terminate the string
    buffer[bytesRead] = '\0';

    printf("[OK] Da doc / Read: %lu bytes\n\n", bytesRead);
    printf("--- NOI DUNG FILE / FILE CONTENTS ---\n");
    printf("%s", buffer);
    printf("--- HET / END ---\n\n");

    delete[] buffer;
    CloseHandle(hFile);
    printf("[OK] Da dong file sau khi doc.\n");
    printf("     File closed after reading.\n\n");

    return TRUE;
}

// ============================================================================
// PHẦN 3: Đọc file từng phần (chunk-by-chunk)
// PART 3: Read file in chunks
// ============================================================================
BOOL ReadFileInChunks(const char* filename) {
    printf("=== PHAN 3: DOC FILE TUNG PHAN / READ FILE IN CHUNKS ===\n\n");

    /*
     * Kỹ thuật này quan trọng khi đọc file lớn
     * This technique is important when reading large files
     * Thay vì đọc toàn bộ file vào bộ nhớ, ta đọc từng phần nhỏ
     * Instead of reading the entire file into memory, we read small chunks
     */

    HANDLE hFile = CreateFileA(
        filename, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateFile (chunk read)");
        return FALSE;
    }

    const DWORD CHUNK_SIZE = 32; // Đọc 32 byte mỗi lần (nhỏ để minh họa)
                                  // Read 32 bytes at a time (small for demo)
    char chunk[CHUNK_SIZE + 1];
    DWORD bytesRead = 0;
    int chunkNumber = 0;
    DWORD totalBytesRead = 0;

    printf("[INFO] Doc file voi kich thuoc chunk = %lu bytes\n", CHUNK_SIZE);
    printf("       Reading file with chunk size = %lu bytes\n\n", CHUNK_SIZE);

    /*
     * Vòng lặp đọc: tiếp tục cho đến khi ReadFile trả về bytesRead = 0
     * Read loop: continue until ReadFile returns bytesRead = 0
     */
    while (TRUE) {
        BOOL success = ReadFile(hFile, chunk, CHUNK_SIZE, &bytesRead, NULL);

        if (!success) {
            PrintError("ReadFile (chunk)");
            CloseHandle(hFile);
            return FALSE;
        }

        // bytesRead = 0 nghĩa là đã đến cuối file (EOF)
        // bytesRead = 0 means we've reached end of file (EOF)
        if (bytesRead == 0) {
            printf("\n[INFO] Da den cuoi file (EOF).\n");
            printf("       Reached end of file (EOF).\n");
            break;
        }

        chunkNumber++;
        totalBytesRead += bytesRead;
        chunk[bytesRead] = '\0';

        printf("  Chunk %d (%lu bytes): [", chunkNumber, bytesRead);
        // In ra, thay thế ký tự xuống dòng để dễ nhìn
        // Print, replacing newline characters for readability
        for (DWORD i = 0; i < bytesRead; i++) {
            if (chunk[i] == '\r') printf("\\r");
            else if (chunk[i] == '\n') printf("\\n");
            else putchar(chunk[i]);
        }
        printf("]\n");
    }

    printf("\n[OK] Tong cong da doc / Total read: %lu bytes trong %d chunks\n\n",
           totalBytesRead, chunkNumber);

    CloseHandle(hFile);
    return TRUE;
}

// ============================================================================
// PHẦN 4: Ghi thêm vào file (Append)
// PART 4: Append to a file
// ============================================================================
BOOL AppendToFile(const char* filename) {
    printf("=== PHAN 4: GHI THEM VAO FILE / APPEND TO FILE ===\n\n");

    /*
     * Để ghi thêm vào cuối file (append), ta mở file với OPEN_EXISTING
     * và sử dụng FILE_APPEND_DATA thay vì GENERIC_WRITE
     *
     * To append to a file, open with OPEN_EXISTING
     * and use FILE_APPEND_DATA instead of GENERIC_WRITE
     *
     * Cách khác: Mở với GENERIC_WRITE + OPEN_EXISTING, rồi dùng
     * SetFilePointer() để di chuyển con trỏ đến cuối file
     *
     * Alternative: Open with GENERIC_WRITE + OPEN_EXISTING, then use
     * SetFilePointer() to move the pointer to end of file
     */
    HANDLE hFile = CreateFileA(
        filename,
        FILE_APPEND_DATA,           // Quyền ghi thêm / Append access
        0,
        NULL,
        OPEN_EXISTING,              // File phải tồn tại / Must exist
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateFile (append)");
        return FALSE;
    }

    const char* appendText = "Dong 6: Dong nay duoc ghi them (appended)!\r\n";
    DWORD bytesWritten = 0;

    BOOL success = WriteFile(
        hFile, appendText, (DWORD)strlen(appendText), &bytesWritten, NULL
    );

    if (!success) {
        PrintError("WriteFile (append)");
        CloseHandle(hFile);
        return FALSE;
    }

    printf("[OK] Da ghi them %lu bytes vao cuoi file.\n", bytesWritten);
    printf("     Appended %lu bytes to end of file.\n\n", bytesWritten);

    CloseHandle(hFile);
    return TRUE;
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH / MAIN PROGRAM
// ============================================================================
int main() {
    printf("************************************************************\n");
    printf("* BAI TAP 01: TAO, GHI VA DOC FILE                        *\n");
    printf("* Exercise 01: Create, Write and Read Files                *\n");
    printf("************************************************************\n\n");

    const char* testFile = "test_output.txt";

    // Bước 1: Tạo và ghi file / Step 1: Create and write file
    if (!CreateAndWriteFile(testFile)) {
        printf(">> Khong the tao/ghi file. Dung chuong trinh.\n");
        printf(">> Cannot create/write file. Stopping.\n");
        return 1;
    }

    // Bước 2: Đọc toàn bộ file / Step 2: Read entire file
    if (!ReadFileContents(testFile)) {
        printf(">> Khong the doc file. Dung chuong trinh.\n");
        printf(">> Cannot read file. Stopping.\n");
        return 1;
    }

    // Bước 3: Đọc file từng phần / Step 3: Read file in chunks
    if (!ReadFileInChunks(testFile)) {
        printf(">> Khong the doc file theo chunk.\n");
        printf(">> Cannot read file in chunks.\n");
        return 1;
    }

    // Bước 4: Ghi thêm vào file / Step 4: Append to file
    if (!AppendToFile(testFile)) {
        printf(">> Khong the ghi them vao file.\n");
        printf(">> Cannot append to file.\n");
        return 1;
    }

    // Bước 5: Đọc lại để xác nhận / Step 5: Read again to verify
    printf("=== PHAN 5: DOC LAI SAU KHI GHI THEM / READ AFTER APPEND ===\n\n");
    ReadFileContents(testFile);

    printf("************************************************************\n");
    printf("* HOAN THANH! File test duoc luu tai: %s\n", testFile);
    printf("* DONE! Test file saved at: %s\n", testFile);
    printf("************************************************************\n");

    return 0;
}
