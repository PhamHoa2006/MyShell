/*
 * ============================================================================
 * BÀI TẬP 03: SAO CHÉP, DI CHUYỂN VÀ XÓA FILE
 * (Copy, Move & Delete Files)
 * ============================================================================
 * Mô tả / Description:
 *   Bài tập này minh họa các thao tác quản lý file: sao chép, di chuyển
 *   (đổi tên), và xóa file. Đây là các thao tác tương đương với lệnh
 *   copy, move/rename, del trong Command Prompt.
 *
 *   This exercise demonstrates file management operations: copy, move
 *   (rename), and delete. These correspond to the copy, move/rename,
 *   and del commands in Command Prompt.
 *
 * Các API chính / Key APIs:
 *   - CopyFileA()     : Sao chép file / Copy a file
 *   - MoveFileA()     : Di chuyển/đổi tên file / Move or rename a file
 *   - MoveFileExA()   : Di chuyển nâng cao (có tùy chọn) / Advanced move
 *   - DeleteFileA()   : Xóa file / Delete a file
 *   - ReplaceFileA()  : Thay thế file an toàn / Safe file replacement
 *
 * Biên dịch / Compile:
 *   g++ 03_copy_move_delete.cpp -o 03_copy_move_delete.exe
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

/* Tạo file test với nội dung / Create a test file with content */
BOOL CreateTestFile(const char* filename, const char* content) {
    HANDLE hFile = CreateFileA(
        filename, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("CreateTestFile");
        return FALSE;
    }
    DWORD written;
    WriteFile(hFile, content, (DWORD)strlen(content), &written, NULL);
    CloseHandle(hFile);
    return TRUE;
}

/* Kiểm tra file tồn tại / Check if file exists */
BOOL FileExists(const char* filename) {
    DWORD attrs = GetFileAttributesA(filename);
    return (attrs != INVALID_FILE_ATTRIBUTES &&
            !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

/* In trạng thái tồn tại / Print existence status */
void PrintFileStatus(const char* label, const char* filename) {
    if (FileExists(filename)) {
        HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,
                                   NULL, OPEN_EXISTING, 0, NULL);
        DWORD size = 0;
        if (hFile != INVALID_HANDLE_VALUE) {
            size = GetFileSize(hFile, NULL);
            CloseHandle(hFile);
        }
        printf("  %s: [TON TAI/EXISTS] %s (%lu bytes)\n", label, filename, size);
    } else {
        printf("  %s: [KHONG CO/MISSING] %s\n", label, filename);
    }
}

// ============================================================================
// PHẦN 1: Sao chép file với CopyFile
// PART 1: Copy files with CopyFile
// ============================================================================
void DemoCopyFile() {
    printf("=== PHAN 1: SAO CHEP FILE / COPY FILE ===\n\n");

    const char* srcFile = "copy_source.txt";
    const char* dstFile = "copy_dest.txt";

    // Tạo file nguồn / Create source file
    CreateTestFile(srcFile, "Noi dung file nguon / Source file content.\r\n"
                            "Dong thu hai / Second line.\r\n");
    printf("[SETUP] Da tao file nguon / Created source file.\n");
    PrintFileStatus("Nguon/Source", srcFile);
    PrintFileStatus("Dich/Dest  ", dstFile);
    printf("\n");

    /*
     * CopyFileA() - Sao chép file
     * CopyFileA() - Copy a file
     *
     * Tham số / Parameters:
     *   lpExistingFileName : Đường dẫn file nguồn / Source file path
     *   lpNewFileName      : Đường dẫn file đích / Destination file path
     *   bFailIfExists      : TRUE = lỗi nếu file đích đã tồn tại
     *                        TRUE = fail if destination already exists
     *                        FALSE = ghi đè file đích
     *                        FALSE = overwrite destination
     *
     * Lưu ý / Notes:
     *   - Thuộc tính bảo mật KHÔNG được sao chép / Security attributes NOT copied
     *   - Thuộc tính file (hidden, readonly) ĐƯỢC sao chép / File attributes ARE copied
     *   - File đích sẽ có thời gian tạo mới / Dest gets new creation time
     */

    // --- Copy bình thường / Normal copy ---
    printf("[1a] Copy binh thuong / Normal copy...\n");
    if (CopyFileA(srcFile, dstFile, FALSE)) {
        printf("  [OK] Sao chep thanh cong!\n");
        printf("       Copy successful!\n");
    } else {
        PrintError("CopyFileA");
    }
    PrintFileStatus("Nguon/Source", srcFile);
    PrintFileStatus("Dich/Dest  ", dstFile);
    printf("\n");

    // --- Copy khi file đích đã tồn tại với bFailIfExists=TRUE ---
    // --- Copy when dest exists with bFailIfExists=TRUE ---
    printf("[1b] Copy voi bFailIfExists=TRUE (file dich da ton tai)...\n");
    printf("     Copy with bFailIfExists=TRUE (dest already exists)...\n");
    if (CopyFileA(srcFile, dstFile, TRUE)) {
        printf("  [OK] Sao chep thanh cong! (bat ngo / unexpected)\n");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] That bai vi file dich da ton tai!\n");
        printf("                      Failed because dest already exists!\n");
        printf("  Ma loi / Error code: %lu (ERROR_FILE_EXISTS = 80)\n", err);
    }
    printf("\n");

    // --- Copy khi file nguồn không tồn tại ---
    // --- Copy when source doesn't exist ---
    printf("[1c] Copy file khong ton tai / Copy non-existent file...\n");
    if (CopyFileA("khong_co.txt", "bat_ky.txt", FALSE)) {
        printf("  [OK] Sao chep thanh cong! (bat ngo / unexpected)\n");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] That bai vi file nguon khong ton tai!\n");
        printf("                      Failed because source doesn't exist!\n");
        printf("  Ma loi / Error code: %lu (ERROR_FILE_NOT_FOUND = 2)\n", err);
    }
    printf("\n");

    // Dọn dẹp / Cleanup
    DeleteFileA(srcFile);
    DeleteFileA(dstFile);
}

// ============================================================================
// PHẦN 2: Di chuyển / đổi tên file với MoveFile
// PART 2: Move / rename files with MoveFile
// ============================================================================
void DemoMoveFile() {
    printf("=== PHAN 2: DI CHUYEN / DOI TEN FILE / MOVE & RENAME ===\n\n");

    const char* origFile = "move_original.txt";
    const char* newName  = "move_renamed.txt";

    CreateTestFile(origFile, "File se duoc doi ten / File to be renamed.\r\n");
    printf("[SETUP] Da tao file: %s\n", origFile);
    PrintFileStatus("Truoc/Before", origFile);
    PrintFileStatus("Sau/After   ", newName);
    printf("\n");

    /*
     * MoveFileA() - Di chuyển hoặc đổi tên file/thư mục
     * MoveFileA() - Move or rename a file/directory
     *
     * Tham số / Parameters:
     *   lpExistingFileName : Tên cũ / Old name (source)
     *   lpNewFileName      : Tên mới / New name (destination)
     *
     * Lưu ý quan trọng / Important notes:
     *   - Nếu nguồn và đích cùng thư mục -> đổi tên / Same dir -> rename
     *   - Nếu khác thư mục, cùng ổ đĩa -> di chuyển nhanh (chỉ đổi metadata)
     *     Different dir, same volume -> fast move (metadata change only)
     *   - Nếu khác ổ đĩa -> CẦN dùng MoveFileEx với MOVEFILE_COPY_ALLOWED
     *     Different volumes -> NEED MoveFileEx with MOVEFILE_COPY_ALLOWED
     *   - Không thể ghi đè file đích đã tồn tại (dùng MoveFileEx)
     *     Cannot overwrite existing dest (use MoveFileEx)
     */

    // --- Đổi tên file / Rename file ---
    printf("[2a] Doi ten file / Rename file...\n");
    if (MoveFileA(origFile, newName)) {
        printf("  [OK] Doi ten thanh cong: %s -> %s\n", origFile, newName);
        printf("       Rename successful!\n");
    } else {
        PrintError("MoveFileA (rename)");
    }
    PrintFileStatus("Ten cu/Old ", origFile);
    PrintFileStatus("Ten moi/New", newName);
    printf("\n");

    // --- MoveFileEx: Di chuyển nâng cao / Advanced move ---
    /*
     * MoveFileExA() - Phiên bản nâng cao với nhiều tùy chọn
     * MoveFileExA() - Advanced version with more options
     *
     * dwFlags:
     *   MOVEFILE_REPLACE_EXISTING : Ghi đè nếu file đích tồn tại
     *                               Overwrite if destination exists
     *   MOVEFILE_COPY_ALLOWED     : Cho phép copy+delete khi khác ổ đĩa
     *                               Allow copy+delete across volumes
     *   MOVEFILE_DELAY_UNTIL_REBOOT : Di chuyển khi khởi động lại
     *                                 Move on next reboot (system files)
     *   MOVEFILE_WRITE_THROUGH    : Đảm bảo ghi hoàn tất trước khi trả về
     *                               Ensure write completes before returning
     */
    printf("[2b] MoveFileEx voi MOVEFILE_REPLACE_EXISTING...\n");

    // Tạo file đích để test ghi đè / Create dest file to test overwrite
    CreateTestFile(origFile, "File moi / New file to overwrite dest.\r\n");

    if (MoveFileExA(origFile, newName, MOVEFILE_REPLACE_EXISTING)) {
        printf("  [OK] Di chuyen + ghi de thanh cong!\n");
        printf("       Move + overwrite successful!\n");
    } else {
        PrintError("MoveFileExA");
    }
    PrintFileStatus("Nguon/Source", origFile);
    PrintFileStatus("Dich/Dest  ", newName);
    printf("\n");

    // --- Di chuyển vào thư mục con / Move into subdirectory ---
    printf("[2c] Di chuyen vao thu muc con / Move into subdirectory...\n");
    CreateDirectoryA("move_test_dir", NULL);
    CreateTestFile(newName, "File se duoc di chuyen / File to be moved.\r\n");

    char destPath[MAX_PATH];
    snprintf(destPath, sizeof(destPath), "move_test_dir\\%s", newName);

    if (MoveFileA(newName, destPath)) {
        printf("  [OK] Di chuyen thanh cong: %s -> %s\n", newName, destPath);
    } else {
        PrintError("MoveFileA (to subdir)");
    }
    PrintFileStatus("Cu/Old ", newName);
    PrintFileStatus("Moi/New", destPath);
    printf("\n");

    // Dọn dẹp / Cleanup
    DeleteFileA(destPath);
    DeleteFileA(newName);
    RemoveDirectoryA("move_test_dir");
}

// ============================================================================
// PHẦN 3: Xóa file với DeleteFile
// PART 3: Delete files with DeleteFile
// ============================================================================
void DemoDeleteFile() {
    printf("=== PHAN 3: XOA FILE / DELETE FILE ===\n\n");

    const char* fileToDelete = "delete_me.txt";

    CreateTestFile(fileToDelete, "File nay se bi xoa / This file will be deleted.\r\n");
    printf("[SETUP] Da tao file: %s\n", fileToDelete);
    PrintFileStatus("Truoc/Before", fileToDelete);
    printf("\n");

    /*
     * DeleteFileA() - Xóa file
     * DeleteFileA() - Delete a file
     *
     * Tham số / Parameters:
     *   lpFileName : Tên file cần xóa / File name to delete
     *
     * Lưu ý / Notes:
     *   - Không xóa được file đang mở (có handle) / Cannot delete open files
     *   - Không xóa được file chỉ đọc / Cannot delete read-only files
     *   - Không xóa được thư mục (dùng RemoveDirectory)
     *     Cannot delete directories (use RemoveDirectory)
     *   - File bị đánh dấu xóa, thực sự bị xóa khi handle cuối cùng đóng
     *     File is marked for deletion, actually removed when last handle closes
     */

    // --- Xóa file bình thường / Normal delete ---
    printf("[3a] Xoa file binh thuong / Normal delete...\n");
    if (DeleteFileA(fileToDelete)) {
        printf("  [OK] Da xoa thanh cong / Deleted successfully!\n");
    } else {
        PrintError("DeleteFileA");
    }
    PrintFileStatus("Sau/After", fileToDelete);
    printf("\n");

    // --- Xóa file không tồn tại / Delete non-existent file ---
    printf("[3b] Xoa file khong ton tai / Delete non-existent file...\n");
    if (DeleteFileA("khong_ton_tai.txt")) {
        printf("  [OK] Xoa thanh cong! (bat ngo / unexpected)\n");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] Khong the xoa - file khong ton tai!\n");
        printf("  Ma loi / Error code: %lu (ERROR_FILE_NOT_FOUND = 2)\n", err);
    }
    printf("\n");

    // --- Xóa file chỉ đọc (sẽ thất bại) / Delete read-only file (will fail) ---
    printf("[3c] Xoa file chi doc / Delete read-only file...\n");
    const char* readonlyFile = "readonly_test.txt";
    CreateTestFile(readonlyFile, "Read-only file.\r\n");
    // Đặt thuộc tính chỉ đọc / Set read-only attribute
    SetFileAttributesA(readonlyFile, FILE_ATTRIBUTE_READONLY);

    if (DeleteFileA(readonlyFile)) {
        printf("  [OK] Xoa thanh cong! (bat ngo / unexpected)\n");
    } else {
        DWORD err = GetLastError();
        printf("  [DUNG ROI/EXPECTED] Khong the xoa file chi doc!\n");
        printf("                      Cannot delete read-only file!\n");
        printf("  Ma loi / Error code: %lu (ERROR_ACCESS_DENIED = 5)\n", err);
    }

    // Phải xóa thuộc tính readonly trước khi xóa
    // Must remove readonly attribute before deleting
    printf("\n  -> Xoa thuoc tinh readonly roi xoa lai...\n");
    printf("     Remove readonly attribute then delete again...\n");
    SetFileAttributesA(readonlyFile, FILE_ATTRIBUTE_NORMAL);
    if (DeleteFileA(readonlyFile)) {
        printf("  [OK] Da xoa thanh cong sau khi bo readonly!\n");
        printf("       Deleted successfully after removing readonly!\n");
    } else {
        PrintError("DeleteFileA (after remove readonly)");
    }
    printf("\n");

    // --- Xóa file đang mở (sẽ thất bại) / Delete open file (will fail) ---
    printf("[3d] Xoa file dang mo / Delete an open file...\n");
    const char* openFile = "open_file_test.txt";
    CreateTestFile(openFile, "File dang mo / Open file.\r\n");

    // Mở file với sharing mode không cho phép xóa
    // Open file with share mode that doesn't allow delete
    HANDLE hFile = CreateFileA(
        openFile, GENERIC_READ, 0, NULL,  // dwShareMode = 0 (exclusive)
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        if (DeleteFileA(openFile)) {
            printf("  [OK] Xoa thanh cong khi file dang mo! (bat ngo)\n");
        } else {
            DWORD err = GetLastError();
            printf("  [DUNG ROI/EXPECTED] Khong the xoa file dang mo!\n");
            printf("                      Cannot delete an open file!\n");
            printf("  Ma loi / Error code: %lu\n", err);
        }
        CloseHandle(hFile);
        DeleteFileA(openFile); // Xóa sau khi đóng / Delete after closing
        printf("  -> Da dong va xoa file. / Closed and deleted file.\n");
    }
    printf("\n");
}

// ============================================================================
// PHẦN 4: ReplaceFile - Thay thế file an toàn
// PART 4: ReplaceFile - Safe file replacement
// ============================================================================
void DemoReplaceFile() {
    printf("=== PHAN 4: THAY THE FILE AN TOAN / SAFE FILE REPLACE ===\n\n");

    /*
     * ReplaceFileA() - Thay thế file một cách an toàn (atomic)
     * ReplaceFileA() - Replace a file safely (atomic operation)
     *
     * Tại sao dùng ReplaceFile thay vì Delete + Move?
     * Why use ReplaceFile instead of Delete + Move?
     *   - An toàn hơn: nếu lỗi xảy ra, file gốc không bị mất
     *     Safer: if error occurs, original file is preserved
     *   - Giữ nguyên thuộc tính bảo mật và alternate data streams
     *     Preserves security attributes and alternate data streams
     *   - Có thể tạo bản sao lưu tự động / Can auto-create backup
     *
     * Tham số / Parameters:
     *   lpReplacedFileName  : File bị thay thế / File to be replaced
     *   lpReplacementFileName : File thay thế / Replacement file
     *   lpBackupFileName    : File sao lưu (có thể NULL) / Backup (can be NULL)
     *   dwReplaceFlags      : Cờ tùy chọn / Flags
     *   lpExclude, lpReserved : Không dùng (NULL) / Not used (NULL)
     */

    const char* origFile    = "replace_original.txt";
    const char* replaceFile = "replace_new.txt";
    const char* backupFile  = "replace_backup.txt";

    CreateTestFile(origFile, "Noi dung cu / Old content.\r\n");
    CreateTestFile(replaceFile, "Noi dung moi / New content.\r\n");

    printf("  Truoc khi thay the / Before replacement:\n");
    PrintFileStatus("File goc/Original    ", origFile);
    PrintFileStatus("File thay the/Replace", replaceFile);
    PrintFileStatus("File sao luu/Backup  ", backupFile);
    printf("\n");

    if (ReplaceFileA(origFile, replaceFile, backupFile, 0, NULL, NULL)) {
        printf("  [OK] Thay the thanh cong! / Replacement successful!\n\n");
        printf("  Sau khi thay the / After replacement:\n");
        PrintFileStatus("File goc/Original    ", origFile);
        PrintFileStatus("File thay the/Replace", replaceFile);
        PrintFileStatus("File sao luu/Backup  ", backupFile);

        // Đọc nội dung file gốc sau thay thế
        // Read original file content after replacement
        HANDLE hFile = CreateFileA(origFile, GENERIC_READ, FILE_SHARE_READ,
                                   NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            char buf[256];
            DWORD bytesRead;
            ReadFile(hFile, buf, sizeof(buf) - 1, &bytesRead, NULL);
            buf[bytesRead] = '\0';
            printf("\n  Noi dung file goc bay gio / Original file now contains:\n");
            printf("  -> %s", buf);
            CloseHandle(hFile);
        }
    } else {
        PrintError("ReplaceFileA");
    }

    // Dọn dẹp / Cleanup
    DeleteFileA(origFile);
    DeleteFileA(replaceFile);
    DeleteFileA(backupFile);
    printf("\n");
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH / MAIN PROGRAM
// ============================================================================
int main() {
    printf("************************************************************\n");
    printf("* BAI TAP 03: SAO CHEP, DI CHUYEN VA XOA FILE             *\n");
    printf("* Exercise 03: Copy, Move & Delete Files                   *\n");
    printf("************************************************************\n\n");

    DemoCopyFile();
    DemoMoveFile();
    DemoDeleteFile();
    DemoReplaceFile();

    printf("************************************************************\n");
    printf("* HOAN THANH! / DONE!                                      *\n");
    printf("************************************************************\n");

    return 0;
}
