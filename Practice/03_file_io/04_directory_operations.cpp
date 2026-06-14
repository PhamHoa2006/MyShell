/*
 * ============================================================================
 * BÀI TẬP 04: THAO TÁC THƯ MỤC (Directory Operations)
 * ============================================================================
 * Mô tả / Description:
 *   Bài tập này minh họa cách tạo, xóa, và điều hướng thư mục trên Windows.
 *   Các thao tác tương đương với lệnh mkdir, rmdir, cd trong Command Prompt.
 *
 *   This exercise demonstrates how to create, remove, and navigate
 *   directories on Windows. These operations correspond to mkdir, rmdir,
 *   and cd commands in Command Prompt.
 *
 * Các API chính / Key APIs:
 *   - CreateDirectoryA()      : Tạo thư mục / Create a directory
 *   - RemoveDirectoryA()      : Xóa thư mục (phải trống) / Remove empty directory
 *   - GetCurrentDirectoryA()  : Lấy thư mục hiện tại / Get current directory
 *   - SetCurrentDirectoryA()  : Đổi thư mục hiện tại / Change current directory
 *   - GetTempPathA()          : Lấy đường dẫn thư mục tạm / Get temp directory
 *   - GetFullPathNameA()      : Chuyển đường dẫn tương đối -> tuyệt đối
 *                               Convert relative path to absolute path
 *
 * Biên dịch / Compile:
 *   g++ 04_directory_operations.cpp -o 04_directory_operations.exe
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

/* In thư mục hiện tại / Print current directory */
void PrintCurrentDir(const char* label) {
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    printf("  %s: %s\n", label, cwd);
}

// ============================================================================
// PHẦN 1: Lấy thư mục hiện tại và các đường dẫn đặc biệt
// PART 1: Get current directory and special paths
// ============================================================================
void DemoGetDirectories() {
    printf("=== PHAN 1: CAC DUONG DAN QUAN TRONG / IMPORTANT PATHS ===\n\n");

    // --- Thư mục hiện tại / Current directory ---
    /*
     * GetCurrentDirectoryA() - Lấy đường dẫn thư mục làm việc hiện tại
     * GetCurrentDirectoryA() - Get current working directory path
     *
     * Tham số / Parameters:
     *   nBufferLength : Kích thước bộ đệm / Buffer size
     *   lpBuffer      : Bộ đệm nhận đường dẫn / Buffer for path
     *
     * Trả về số ký tự đã ghi (không tính null)
     * Returns number of characters written (excluding null)
     * Nếu bộ đệm quá nhỏ, trả về kích thước cần thiết
     * If buffer too small, returns required size
     */
    char currentDir[MAX_PATH];
    DWORD len = GetCurrentDirectoryA(MAX_PATH, currentDir);
    if (len > 0) {
        printf("  Thu muc hien tai / Current Directory:\n");
        printf("    %s\n", currentDir);
        printf("    (Do dai / Length: %lu ky tu / chars)\n\n", len);
    } else {
        PrintError("GetCurrentDirectoryA");
    }

    // --- Thư mục tạm / Temp directory ---
    /*
     * GetTempPathA() - Lấy đường dẫn thư mục tạm của hệ thống
     * GetTempPathA() - Get system temporary directory path
     *
     * Windows tìm thư mục tạm theo thứ tự ưu tiên:
     * Windows searches for temp dir in this priority order:
     *   1. Biến môi trường TMP / TMP environment variable
     *   2. Biến môi trường TEMP / TEMP environment variable
     *   3. Biến môi trường USERPROFILE / USERPROFILE environment variable
     *   4. Thư mục Windows / Windows directory
     */
    char tempDir[MAX_PATH];
    DWORD tempLen = GetTempPathA(MAX_PATH, tempDir);
    if (tempLen > 0) {
        printf("  Thu muc tam / Temp Directory:\n");
        printf("    %s\n\n", tempDir);
    }

    // --- Thư mục Windows / Windows directory ---
    char winDir[MAX_PATH];
    GetWindowsDirectoryA(winDir, MAX_PATH);
    printf("  Thu muc Windows / Windows Directory:\n");
    printf("    %s\n\n", winDir);

    // --- Thư mục System / System directory ---
    char sysDir[MAX_PATH];
    GetSystemDirectoryA(sysDir, MAX_PATH);
    printf("  Thu muc System / System Directory:\n");
    printf("    %s\n\n", sysDir);

    // --- Chuyển đường dẫn tương đối -> tuyệt đối ---
    // --- Convert relative to absolute path ---
    /*
     * GetFullPathNameA() - Chuyển đường dẫn tương đối thành tuyệt đối
     * GetFullPathNameA() - Convert relative path to absolute path
     *
     * Rất hữu ích khi cần chuẩn hóa đường dẫn do người dùng nhập
     * Very useful for normalizing user-input paths
     */
    char fullPath[MAX_PATH];
    char* filePart = NULL;
    GetFullPathNameA("..\\test.txt", MAX_PATH, fullPath, &filePart);
    printf("  Duong dan tuong doi / Relative: ..\\test.txt\n");
    printf("  Duong dan tuyet doi / Absolute: %s\n", fullPath);
    if (filePart) {
        printf("  Phan ten file / File part:      %s\n", filePart);
    }
    printf("\n");
}

// ============================================================================
// PHẦN 2: Tạo thư mục
// PART 2: Create directories
// ============================================================================
void DemoCreateDirectory() {
    printf("=== PHAN 2: TAO THU MUC / CREATE DIRECTORIES ===\n\n");

    // --- Tạo một thư mục đơn / Create a single directory ---
    /*
     * CreateDirectoryA() - Tạo thư mục mới
     * CreateDirectoryA() - Create a new directory
     *
     * Tham số / Parameters:
     *   lpPathName          : Đường dẫn thư mục / Directory path
     *   lpSecurityAttributes : Thuộc tính bảo mật (NULL = mặc định)
     *                         Security attributes (NULL = default)
     *
     * Lưu ý / Notes:
     *   - Chỉ tạo được thư mục cuối cùng trong đường dẫn
     *     Can only create the last directory in the path
     *   - Thư mục cha phải tồn tại / Parent directory must exist
     *   - Lỗi nếu thư mục đã tồn tại / Error if directory already exists
     */
    printf("[2a] Tao thu muc don / Create single directory...\n");
    if (CreateDirectoryA("dir_test", NULL)) {
        printf("  [OK] Da tao: dir_test\n");
    } else {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS) {
            printf("  [INFO] Thu muc da ton tai / Directory already exists: dir_test\n");
        } else {
            PrintError("CreateDirectoryA");
        }
    }

    // --- Tạo thư mục con / Create nested subdirectory ---
    printf("[2b] Tao thu muc con / Create subdirectory...\n");
    if (CreateDirectoryA("dir_test\\sub1", NULL)) {
        printf("  [OK] Da tao: dir_test\\sub1\n");
    } else {
        PrintError("CreateDirectoryA (sub1)");
    }

    // --- Thử tạo thư mục sâu nhiều cấp (sẽ thất bại) ---
    // --- Try creating deeply nested directory (will fail) ---
    printf("[2c] Thu tao thu muc sau nhieu cap (se that bai)...\n");
    printf("     Try creating deeply nested dir (will fail)...\n");
    if (CreateDirectoryA("dir_test\\a\\b\\c", NULL)) {
        printf("  [OK] Da tao! (bat ngo / unexpected)\n");
    } else {
        printf("  [DUNG ROI/EXPECTED] Khong the tao - thu muc cha chua ton tai!\n");
        printf("                      Cannot create - parent dirs don't exist!\n");
    }

    // --- Hàm tự tạo thư mục đệ quy (tương đương mkdir -p) ---
    // --- Custom recursive directory creation (like mkdir -p) ---
    printf("\n[2d] Tao thu muc de quy (mkdir -p) / Recursive mkdir...\n");
    printf("     Tao: dir_test\\level1\\level2\\level3\n\n");

    /*
     * Windows không có API tạo thư mục đệ quy như mkdir -p
     * Windows doesn't have a recursive mkdir API like mkdir -p
     * Ta phải tự viết bằng cách tạo từng cấp thư mục
     * We need to create each level manually
     */
    const char* nestedPaths[] = {
        "dir_test\\level1",
        "dir_test\\level1\\level2",
        "dir_test\\level1\\level2\\level3"
    };

    for (int i = 0; i < 3; i++) {
        if (CreateDirectoryA(nestedPaths[i], NULL)) {
            printf("  [OK] Da tao / Created: %s\n", nestedPaths[i]);
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_ALREADY_EXISTS) {
                printf("  [--] Da ton tai / Exists: %s\n", nestedPaths[i]);
            } else {
                PrintError(nestedPaths[i]);
            }
        }
    }
    printf("\n");
}

// ============================================================================
// PHẦN 3: Điều hướng thư mục (cd)
// PART 3: Navigate directories (cd)
// ============================================================================
void DemoNavigateDirectories() {
    printf("=== PHAN 3: DIEU HUONG THU MUC / NAVIGATE DIRECTORIES ===\n\n");

    // Lưu thư mục hiện tại để khôi phục sau
    // Save current directory to restore later
    char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    PrintCurrentDir("Ban dau / Start");

    /*
     * SetCurrentDirectoryA() - Đổi thư mục làm việc hiện tại
     * SetCurrentDirectoryA() - Change current working directory
     *
     * Tương đương lệnh cd trong Command Prompt
     * Equivalent to the cd command in Command Prompt
     *
     * Lưu ý quan trọng / Important note:
     *   Thư mục làm việc là thuộc tính của TIẾN TRÌNH, không phải luồng
     *   Current directory is a PROCESS attribute, not thread-local
     *   Thay đổi ở một luồng sẽ ảnh hưởng tất cả các luồng trong tiến trình
     *   Changes in one thread affect all threads in the process
     */

    // --- Di chuyển vào thư mục con / Move into subdirectory ---
    printf("\n[3a] cd dir_test\\level1\\level2\n");
    if (SetCurrentDirectoryA("dir_test\\level1\\level2")) {
        PrintCurrentDir("Hien tai / Current");
    } else {
        PrintError("SetCurrentDirectoryA (into subdir)");
    }

    // --- Di chuyển lên thư mục cha / Move to parent directory ---
    printf("\n[3b] cd .. (len thu muc cha / go to parent)\n");
    if (SetCurrentDirectoryA("..")) {
        PrintCurrentDir("Hien tai / Current");
    } else {
        PrintError("SetCurrentDirectoryA (..)");
    }

    // --- Di chuyển lên 2 cấp / Move up 2 levels ---
    printf("\n[3c] cd ..\\..\n");
    if (SetCurrentDirectoryA("..\\..")) {
        PrintCurrentDir("Hien tai / Current");
    } else {
        PrintError("SetCurrentDirectoryA (../..)");
    }

    // --- Di chuyển đến đường dẫn tuyệt đối / Move to absolute path ---
    printf("\n[3d] cd C:\\Windows\n");
    if (SetCurrentDirectoryA("C:\\Windows")) {
        PrintCurrentDir("Hien tai / Current");
    } else {
        PrintError("SetCurrentDirectoryA (C:\\Windows)");
    }

    // --- Thử di chuyển đến thư mục không tồn tại ---
    // --- Try moving to non-existent directory ---
    printf("\n[3e] cd KhongTonTai (se that bai / will fail)\n");
    if (SetCurrentDirectoryA("KhongTonTai")) {
        PrintCurrentDir("Hien tai / Current");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] Khong the cd - thu muc khong ton tai!\n");
        printf("  Ma loi / Error code: %lu\n", err);
    }

    // --- Khôi phục thư mục ban đầu / Restore original directory ---
    printf("\n[3f] Khoi phuc thu muc ban dau / Restore original directory\n");
    SetCurrentDirectoryA(savedDir);
    PrintCurrentDir("Khoi phuc / Restored");
    printf("\n");
}

// ============================================================================
// PHẦN 4: Xóa thư mục
// PART 4: Remove directories
// ============================================================================
void DemoRemoveDirectory() {
    printf("=== PHAN 4: XOA THU MUC / REMOVE DIRECTORIES ===\n\n");

    /*
     * RemoveDirectoryA() - Xóa thư mục RỖNG
     * RemoveDirectoryA() - Remove an EMPTY directory
     *
     * Lưu ý quan trọng / Important notes:
     *   - Thư mục PHẢI rỗng / Directory MUST be empty
     *   - Không thể xóa thư mục đang là current directory
     *     Cannot remove directory that is current directory
     *   - Không thể xóa thư mục chứa file hoặc thư mục con
     *     Cannot remove directory containing files or subdirectories
     */

    // --- Thử xóa thư mục có nội dung (sẽ thất bại) ---
    // --- Try removing non-empty directory (will fail) ---
    printf("[4a] Xoa thu muc khong rong / Remove non-empty directory...\n");
    if (RemoveDirectoryA("dir_test")) {
        printf("  [OK] Da xoa! (bat ngo / unexpected)\n");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] Khong the xoa - thu muc khong rong!\n");
        printf("                      Cannot remove - directory not empty!\n");
        printf("  Ma loi / Error code: %lu (ERROR_DIR_NOT_EMPTY = 145)\n", err);
    }
    printf("\n");

    // --- Xóa thư mục đệ quy (từ trong ra ngoài) ---
    // --- Remove directories recursively (inside-out) ---
    printf("[4b] Xoa thu muc de quy (tu trong ra ngoai)...\n");
    printf("     Recursive removal (inside-out)...\n\n");

    /*
     * Để xóa cây thư mục, ta phải:
     * To remove a directory tree, we must:
     *   1. Xóa tất cả file trong thư mục / Delete all files in the directory
     *   2. Xóa tất cả thư mục con (đệ quy) / Remove all subdirs (recursively)
     *   3. Xóa thư mục hiện tại / Remove current directory
     *
     * Trong bài này ta đã biết cấu trúc, nên xóa thủ công
     * Here we know the structure, so we remove manually
     * Bài 05 sẽ kết hợp FindFirstFile/FindNextFile để xóa đệ quy tự động
     * Exercise 05 will combine FindFirst/FindNext for automatic recursive removal
     */
    const char* dirsToRemove[] = {
        "dir_test\\level1\\level2\\level3",
        "dir_test\\level1\\level2",
        "dir_test\\level1",
        "dir_test\\sub1",
        "dir_test"
    };

    for (int i = 0; i < 5; i++) {
        if (RemoveDirectoryA(dirsToRemove[i])) {
            printf("  [OK] Da xoa / Removed: %s\n", dirsToRemove[i]);
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_PATH_NOT_FOUND) {
                printf("  [--] Khong tim thay / Not found: %s\n", dirsToRemove[i]);
            } else {
                printf("  [!!] Loi xoa / Error removing: %s (code=%lu)\n",
                       dirsToRemove[i], err);
            }
        }
    }
    printf("\n");
}

// ============================================================================
// PHẦN 5: Tạo file tạm thời
// PART 5: Create temporary files
// ============================================================================
void DemoTempFiles() {
    printf("=== PHAN 5: FILE TAM THOI / TEMPORARY FILES ===\n\n");

    /*
     * GetTempFileNameA() - Tạo tên file tạm thời duy nhất
     * GetTempFileNameA() - Generate a unique temporary file name
     *
     * Tham số / Parameters:
     *   lpPathName : Thư mục chứa file tạm / Directory for temp file
     *   lpPrefixString : Tiền tố 3 ký tự / 3-character prefix
     *   uUnique    : 0 = tự tạo tên duy nhất + tạo file
     *                0 = auto-generate unique name AND create the file
     *                != 0 = dùng giá trị này, KHÔNG tạo file
     *                != 0 = use this value, DON'T create the file
     *   lpTempFileName : [out] Bộ đệm nhận tên file / Buffer for filename
     *
     * Lưu ý: Khi uUnique = 0, API thực sự TẠO file rỗng
     * Note: When uUnique = 0, the API actually CREATES an empty file
     */
    char tempDir[MAX_PATH];
    GetTempPathA(MAX_PATH, tempDir);

    char tempFile[MAX_PATH];
    UINT result = GetTempFileNameA(tempDir, "TST", 0, tempFile);

    if (result != 0) {
        printf("  File tam da tao / Temp file created:\n");
        printf("    %s\n\n", tempFile);

        // File đã được tạo tự động, có thể sử dụng ngay
        // File was auto-created, can be used immediately
        HANDLE hFile = CreateFileA(tempFile, GENERIC_WRITE, 0, NULL,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            const char* data = "Du lieu tam / Temp data\r\n";
            DWORD written;
            WriteFile(hFile, data, (DWORD)strlen(data), &written, NULL);
            CloseHandle(hFile);
            printf("  Da ghi %lu bytes vao file tam / Wrote to temp file.\n\n", written);
        }

        // Xóa file tạm khi xong / Delete temp file when done
        DeleteFileA(tempFile);
        printf("  Da xoa file tam / Deleted temp file.\n");
    } else {
        PrintError("GetTempFileNameA");
    }
    printf("\n");
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH / MAIN PROGRAM
// ============================================================================
int main() {
    printf("************************************************************\n");
    printf("* BAI TAP 04: THAO TAC THU MUC                            *\n");
    printf("* Exercise 04: Directory Operations                        *\n");
    printf("************************************************************\n\n");

    DemoGetDirectories();
    DemoCreateDirectory();
    DemoNavigateDirectories();
    DemoRemoveDirectory();
    DemoTempFiles();

    printf("************************************************************\n");
    printf("* HOAN THANH! / DONE!                                      *\n");
    printf("************************************************************\n");

    return 0;
}
