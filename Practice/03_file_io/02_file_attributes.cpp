/*
 * ============================================================================
 * BÀI TẬP 02: THUỘC TÍNH FILE (File Attributes & Metadata)
 * ============================================================================
 * Mô tả / Description:
 *   Bài tập này minh họa cách lấy và hiển thị thông tin chi tiết về file:
 *   kích thước, thời gian tạo/sửa/truy cập, và các thuộc tính đặc biệt.
 *
 *   This exercise demonstrates how to retrieve and display detailed file
 *   information: size, creation/modification/access times, and special
 *   attributes like read-only, hidden, system, etc.
 *
 * Các API chính / Key APIs:
 *   - GetFileAttributesA()         : Lấy thuộc tính file / Get file attributes
 *   - GetFileAttributesExA()       : Lấy thuộc tính mở rộng / Extended attributes
 *   - GetFileSize()                : Lấy kích thước file / Get file size
 *   - GetFileSizeEx()              : Kích thước file 64-bit / 64-bit file size
 *   - GetFileTime()                : Lấy thời gian file / Get file times
 *   - FileTimeToSystemTime()       : Chuyển FILETIME -> SYSTEMTIME
 *   - FileTimeToLocalFileTime()    : Chuyển UTC -> giờ địa phương / UTC to local
 *   - SetFileAttributesA()         : Đặt thuộc tính file / Set file attributes
 *   - GetFileInformationByHandle() : Thông tin đầy đủ / Complete file info
 *
 * Biên dịch / Compile:
 *   g++ 02_file_attributes.cpp -o 02_file_attributes.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Hàm tiện ích / Utility functions
// ============================================================================

void PrintError(const char* operation) {
    DWORD err = GetLastError();
    char msgBuf[512];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
        msgBuf, sizeof(msgBuf), NULL
    );
    printf("[LOI/ERROR] %s - Ma loi/Code: %lu\n  %s\n", operation, err, msgBuf);
}

/*
 * Chuyển đổi FILETIME thành chuỗi dễ đọc
 * Convert FILETIME to a human-readable string
 *
 * FILETIME là cấu trúc 64-bit biểu diễn số khoảng 100 nanosecond
 * kể từ 1/1/1601 (UTC) - đây là chuẩn thời gian của Windows
 *
 * FILETIME is a 64-bit structure representing the number of 100-nanosecond
 * intervals since January 1, 1601 (UTC) - this is the Windows time standard
 */
void FileTimeToString(const FILETIME* ft, char* buffer, size_t bufSize) {
    FILETIME localFT;
    SYSTEMTIME st;

    // Bước 1: Chuyển từ UTC sang giờ địa phương
    // Step 1: Convert from UTC to local time
    FileTimeToLocalFileTime(ft, &localFT);

    // Bước 2: Chuyển FILETIME sang SYSTEMTIME (cấu trúc có năm/tháng/ngày/giờ)
    // Step 2: Convert FILETIME to SYSTEMTIME (struct with year/month/day/hour)
    FileTimeToSystemTime(&localFT, &st);

    // Bước 3: Định dạng thành chuỗi / Step 3: Format as string
    snprintf(buffer, bufSize,
             "%04d-%02d-%02d %02d:%02d:%02d",
             st.wYear, st.wMonth, st.wDay,
             st.wHour, st.wMinute, st.wSecond);
}

/*
 * Chuyển đổi kích thước file thành chuỗi dễ đọc (KB, MB, GB)
 * Convert file size to human-readable string (KB, MB, GB)
 */
void FormatFileSize(ULONGLONG size, char* buffer, size_t bufSize) {
    if (size >= 1073741824ULL) {
        snprintf(buffer, bufSize, "%.2f GB (%llu bytes)", (double)size / 1073741824.0, size);
    } else if (size >= 1048576ULL) {
        snprintf(buffer, bufSize, "%.2f MB (%llu bytes)", (double)size / 1048576.0, size);
    } else if (size >= 1024ULL) {
        snprintf(buffer, bufSize, "%.2f KB (%llu bytes)", (double)size / 1024.0, size);
    } else {
        snprintf(buffer, bufSize, "%llu bytes", size);
    }
}

// ============================================================================
// PHẦN 1: Lấy thuộc tính cơ bản bằng GetFileAttributes
// PART 1: Get basic attributes with GetFileAttributes
// ============================================================================
void ShowBasicAttributes(const char* filename) {
    printf("=== PHAN 1: THUOC TINH CO BAN / BASIC ATTRIBUTES ===\n\n");

    /*
     * GetFileAttributesA() - Lấy thuộc tính file đơn giản nhất
     * GetFileAttributesA() - Simplest way to get file attributes
     *
     * Trả về một bitmask DWORD chứa các cờ thuộc tính
     * Returns a DWORD bitmask containing attribute flags
     *
     * Trả về INVALID_FILE_ATTRIBUTES nếu thất bại
     * Returns INVALID_FILE_ATTRIBUTES on failure
     */
    DWORD attrs = GetFileAttributesA(filename);

    if (attrs == INVALID_FILE_ATTRIBUTES) {
        PrintError("GetFileAttributesA");
        return;
    }

    printf("  File: %s\n", filename);
    printf("  Gia tri thuoc tinh (hex) / Attribute value: 0x%08lX\n\n", attrs);

    /*
     * Phân tích từng bit thuộc tính / Analyze each attribute bit
     * Mỗi thuộc tính là một bit trong bitmask
     * Each attribute is a bit in the bitmask
     */
    printf("  Thuoc tinh / Attributes:\n");

    // FILE_ATTRIBUTE_READONLY (0x01): File chỉ đọc / Read-only file
    printf("    [%c] Chi doc / Read-only       (FILE_ATTRIBUTE_READONLY)\n",
           (attrs & FILE_ATTRIBUTE_READONLY)  ? 'X' : ' ');

    // FILE_ATTRIBUTE_HIDDEN (0x02): File ẩn / Hidden file
    printf("    [%c] An / Hidden               (FILE_ATTRIBUTE_HIDDEN)\n",
           (attrs & FILE_ATTRIBUTE_HIDDEN)    ? 'X' : ' ');

    // FILE_ATTRIBUTE_SYSTEM (0x04): File hệ thống / System file
    printf("    [%c] He thong / System          (FILE_ATTRIBUTE_SYSTEM)\n",
           (attrs & FILE_ATTRIBUTE_SYSTEM)    ? 'X' : ' ');

    // FILE_ATTRIBUTE_DIRECTORY (0x10): Thư mục / Directory
    printf("    [%c] Thu muc / Directory        (FILE_ATTRIBUTE_DIRECTORY)\n",
           (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 'X' : ' ');

    // FILE_ATTRIBUTE_ARCHIVE (0x20): Cần sao lưu / Needs backup
    printf("    [%c] Luu tru / Archive          (FILE_ATTRIBUTE_ARCHIVE)\n",
           (attrs & FILE_ATTRIBUTE_ARCHIVE)   ? 'X' : ' ');

    // FILE_ATTRIBUTE_NORMAL (0x80): Không có thuộc tính đặc biệt
    // Only valid when used alone / Chỉ hợp lệ khi dùng riêng
    printf("    [%c] Binh thuong / Normal        (FILE_ATTRIBUTE_NORMAL)\n",
           (attrs & FILE_ATTRIBUTE_NORMAL)    ? 'X' : ' ');

    // FILE_ATTRIBUTE_TEMPORARY (0x100): File tạm / Temporary file
    printf("    [%c] Tam thoi / Temporary       (FILE_ATTRIBUTE_TEMPORARY)\n",
           (attrs & FILE_ATTRIBUTE_TEMPORARY) ? 'X' : ' ');

    // FILE_ATTRIBUTE_COMPRESSED (0x800): File nén / Compressed
    printf("    [%c] Nen / Compressed           (FILE_ATTRIBUTE_COMPRESSED)\n",
           (attrs & FILE_ATTRIBUTE_COMPRESSED) ? 'X' : ' ');

    // FILE_ATTRIBUTE_ENCRYPTED (0x4000): File mã hóa / Encrypted
    printf("    [%c] Ma hoa / Encrypted         (FILE_ATTRIBUTE_ENCRYPTED)\n",
           (attrs & FILE_ATTRIBUTE_ENCRYPTED) ? 'X' : ' ');

    printf("\n");
}

// ============================================================================
// PHẦN 2: Lấy thông tin chi tiết bằng GetFileInformationByHandle
// PART 2: Get detailed info using GetFileInformationByHandle
// ============================================================================
void ShowDetailedInfo(const char* filename) {
    printf("=== PHAN 2: THONG TIN CHI TIET / DETAILED INFORMATION ===\n\n");

    HANDLE hFile = CreateFileA(
        filename, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateFile");
        return;
    }

    // --- Kích thước file 64-bit / 64-bit file size ---
    /*
     * GetFileSizeEx() - Phiên bản 64-bit của GetFileSize
     * GetFileSizeEx() - 64-bit version of GetFileSize
     *
     * Sử dụng LARGE_INTEGER để lưu kích thước file lên đến 2^63 bytes
     * Uses LARGE_INTEGER to store file sizes up to 2^63 bytes
     *
     * GetFileSize() cũ chỉ hỗ trợ tối đa 4GB (32-bit)
     * Old GetFileSize() only supports up to 4GB (32-bit)
     */
    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(hFile, &fileSize)) {
        char sizeStr[128];
        FormatFileSize((ULONGLONG)fileSize.QuadPart, sizeStr, sizeof(sizeStr));
        printf("  Kich thuoc / Size: %s\n", sizeStr);
    } else {
        PrintError("GetFileSizeEx");
    }

    // --- Thời gian file / File times ---
    /*
     * GetFileTime() - Lấy 3 mốc thời gian của file
     * GetFileTime() - Get 3 timestamps of a file
     *
     *   ftCreationTime   : Thời gian tạo / Creation time
     *   ftLastAccessTime : Thời gian truy cập cuối / Last access time
     *   ftLastWriteTime  : Thời gian sửa đổi cuối / Last modification time
     *
     * Tất cả đều ở dạng UTC, cần chuyển sang local time để hiển thị
     * All are in UTC format, need conversion to local time for display
     */
    FILETIME ftCreate, ftAccess, ftWrite;
    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
        char timeStr[64];

        FileTimeToString(&ftCreate, timeStr, sizeof(timeStr));
        printf("  Thoi gian tao / Created:    %s\n", timeStr);

        FileTimeToString(&ftAccess, timeStr, sizeof(timeStr));
        printf("  Truy cap cuoi / Accessed:   %s\n", timeStr);

        FileTimeToString(&ftWrite, timeStr, sizeof(timeStr));
        printf("  Sua doi cuoi / Modified:    %s\n", timeStr);
    } else {
        PrintError("GetFileTime");
    }

    // --- Thông tin đầy đủ / Complete information ---
    /*
     * GetFileInformationByHandle() - Lấy thông tin đầy đủ nhất về file
     * GetFileInformationByHandle() - Get the most complete file information
     *
     * Cấu trúc BY_HANDLE_FILE_INFORMATION chứa:
     *   - dwFileAttributes: Thuộc tính file
     *   - ftCreationTime, ftLastAccessTime, ftLastWriteTime: Thời gian
     *   - dwVolumeSerialNumber: Số serial của ổ đĩa / Volume serial number
     *   - nFileSizeHigh, nFileSizeLow: Kích thước file 64-bit
     *   - nNumberOfLinks: Số hard link (thường là 1 trên NTFS)
     *   - nFileIndexHigh, nFileIndexLow: ID duy nhất của file trên volume
     *                                    Unique file ID on the volume
     */
    BY_HANDLE_FILE_INFORMATION fileInfo;
    if (GetFileInformationByHandle(hFile, &fileInfo)) {
        printf("\n  --- Thong tin mo rong / Extended Info ---\n");
        printf("  Volume Serial Number : 0x%08lX\n", fileInfo.dwVolumeSerialNumber);
        printf("  So hard link / Links : %lu\n", fileInfo.nNumberOfLinks);

        ULONGLONG fileIndex = ((ULONGLONG)fileInfo.nFileIndexHigh << 32)
                              | fileInfo.nFileIndexLow;
        printf("  File Index (ID)      : 0x%016llX\n", fileIndex);
    } else {
        PrintError("GetFileInformationByHandle");
    }

    CloseHandle(hFile);
    printf("\n");
}

// ============================================================================
// PHẦN 3: Thay đổi thuộc tính file
// PART 3: Modify file attributes
// ============================================================================
void ModifyAttributes(const char* filename) {
    printf("=== PHAN 3: THAY DOI THUOC TINH / MODIFY ATTRIBUTES ===\n\n");

    // Lấy thuộc tính hiện tại / Get current attributes
    DWORD attrs = GetFileAttributesA(filename);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        PrintError("GetFileAttributesA");
        return;
    }
    printf("  Truoc khi thay doi / Before: 0x%08lX\n", attrs);

    // --- Đặt file thành chỉ đọc / Set file as read-only ---
    /*
     * SetFileAttributesA() - Thay đổi thuộc tính file
     * SetFileAttributesA() - Change file attributes
     *
     * Dùng phép OR (|) để thêm thuộc tính
     * Use OR (|) to add attributes
     *
     * Dùng phép AND NOT (& ~) để xóa thuộc tính
     * Use AND NOT (& ~) to remove attributes
     */
    DWORD newAttrs = attrs | FILE_ATTRIBUTE_READONLY;
    if (SetFileAttributesA(filename, newAttrs)) {
        printf("  [OK] Da dat READONLY / Set READONLY\n");
    } else {
        PrintError("SetFileAttributesA (set readonly)");
    }

    // Xác nhận / Verify
    DWORD verifyAttrs = GetFileAttributesA(filename);
    printf("  Sau khi dat / After set: 0x%08lX", verifyAttrs);
    printf(" -> READONLY: %s\n",
           (verifyAttrs & FILE_ATTRIBUTE_READONLY) ? "CO/YES" : "KHONG/NO");

    // --- Thử ghi vào file chỉ đọc (sẽ thất bại) ---
    // --- Try writing to read-only file (will fail) ---
    printf("\n  [TEST] Thu ghi vao file chi doc / Write to read-only file...\n");
    HANDLE hFile = CreateFileA(
        filename, GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("  [DUNG ROI/EXPECTED] Khong the ghi - file chi doc!\n");
        printf("                      Cannot write - file is read-only!\n");
    } else {
        printf("  [BAT NGO/UNEXPECTED] Da mo duoc file de ghi?!\n");
        CloseHandle(hFile);
    }

    // --- Xóa thuộc tính chỉ đọc / Remove read-only attribute ---
    newAttrs = verifyAttrs & ~FILE_ATTRIBUTE_READONLY;
    // Đảm bảo không đặt giá trị 0 (phải có ít nhất NORMAL)
    // Ensure we don't set value 0 (must have at least NORMAL)
    if (newAttrs == 0) newAttrs = FILE_ATTRIBUTE_NORMAL;

    if (SetFileAttributesA(filename, newAttrs)) {
        printf("\n  [OK] Da xoa READONLY / Removed READONLY\n");
    } else {
        PrintError("SetFileAttributesA (remove readonly)");
    }

    verifyAttrs = GetFileAttributesA(filename);
    printf("  Sau khi xoa / After remove: 0x%08lX", verifyAttrs);
    printf(" -> READONLY: %s\n\n",
           (verifyAttrs & FILE_ATTRIBUTE_READONLY) ? "CO/YES" : "KHONG/NO");
}

// ============================================================================
// PHẦN 4: Kiểm tra sự tồn tại của file/thư mục
// PART 4: Check file/directory existence
// ============================================================================
void CheckExistence() {
    printf("=== PHAN 4: KIEM TRA TON TAI / CHECK EXISTENCE ===\n\n");

    /*
     * Mẹo: Dùng GetFileAttributes để kiểm tra file/thư mục tồn tại
     * Tip: Use GetFileAttributes to check if a file/directory exists
     * Nhanh hơn CreateFile vì không cần mở handle
     * Faster than CreateFile because no handle needs to be opened
     */
    const char* paths[] = {
        "C:\\Windows",
        "C:\\Windows\\System32\\notepad.exe",
        "C:\\KhongTonTai\\file.txt",
        "."
    };
    int count = sizeof(paths) / sizeof(paths[0]);

    for (int i = 0; i < count; i++) {
        DWORD attrs = GetFileAttributesA(paths[i]);
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            printf("  [X] %-45s -> KHONG TON TAI / NOT FOUND\n", paths[i]);
        } else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
            printf("  [D] %-45s -> THU MUC / DIRECTORY\n", paths[i]);
        } else {
            printf("  [F] %-45s -> FILE\n", paths[i]);
        }
    }
    printf("\n");
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH / MAIN PROGRAM
// ============================================================================
int main() {
    printf("************************************************************\n");
    printf("* BAI TAP 02: THUOC TINH FILE                              *\n");
    printf("* Exercise 02: File Attributes & Metadata                  *\n");
    printf("************************************************************\n\n");

    // Tạo file test / Create test file
    const char* testFile = "test_attributes.txt";
    HANDLE hFile = CreateFileA(
        testFile, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        const char* data = "Day la file test cho bai tap thuoc tinh.\r\n"
                           "This is a test file for the attributes exercise.\r\n";
        DWORD written;
        WriteFile(hFile, data, (DWORD)strlen(data), &written, NULL);
        CloseHandle(hFile);
        printf("[OK] Da tao file test: %s\n\n", testFile);
    } else {
        PrintError("Tao file test / Create test file");
        return 1;
    }

    // Phần 1: Thuộc tính cơ bản
    ShowBasicAttributes(testFile);

    // Phần 2: Thông tin chi tiết
    ShowDetailedInfo(testFile);

    // Phần 3: Thay đổi thuộc tính
    ModifyAttributes(testFile);

    // Phần 4: Kiểm tra tồn tại
    CheckExistence();

    // Dọn dẹp / Cleanup
    DeleteFileA(testFile);
    printf("[OK] Da xoa file test / Deleted test file: %s\n", testFile);

    printf("\n************************************************************\n");
    printf("* HOAN THANH! / DONE!                                      *\n");
    printf("************************************************************\n");

    return 0;
}
