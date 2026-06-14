/*
 * ============================================================================
 * BÀI TẬP 05: TÌM KIẾM FILE (Find Files)
 * ============================================================================
 * Mô tả / Description:
 *   Bài tập này minh họa cách liệt kê file/thư mục, tìm kiếm với wildcard,
 *   và duyệt cây thư mục đệ quy. Đây là nền tảng cho lệnh `dir` và
 *   `list_tree` trong Tiny Shell.
 *
 *   This exercise demonstrates how to list files/directories, search with
 *   wildcards, and traverse directory trees recursively. This is the
 *   foundation for the `dir` and `list_tree` commands in the Tiny Shell.
 *
 * Các API chính / Key APIs:
 *   - FindFirstFileA() : Bắt đầu tìm kiếm / Start a search
 *   - FindNextFileA()  : Tìm file tiếp theo / Find next file
 *   - FindClose()      : Kết thúc tìm kiếm / End the search
 *   - WIN32_FIND_DATAA : Cấu trúc chứa thông tin file / File info structure
 *
 * Biên dịch / Compile:
 *   g++ 05_find_files.cpp -o 05_find_files.exe
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

/* Chuyển FILETIME thành chuỗi / Convert FILETIME to string */
void FileTimeToStr(const FILETIME* ft, char* buf, size_t bufSize) {
    FILETIME localFT;
    SYSTEMTIME st;
    FileTimeToLocalFileTime(ft, &localFT);
    FileTimeToSystemTime(&localFT, &st);
    snprintf(buf, bufSize, "%04d-%02d-%02d %02d:%02d",
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
}

/* Định dạng kích thước file / Format file size */
void FormatSize(ULONGLONG size, char* buf, size_t bufSize) {
    if (size >= 1073741824ULL)
        snprintf(buf, bufSize, "%8.1f GB", (double)size / 1073741824.0);
    else if (size >= 1048576ULL)
        snprintf(buf, bufSize, "%8.1f MB", (double)size / 1048576.0);
    else if (size >= 1024ULL)
        snprintf(buf, bufSize, "%8.1f KB", (double)size / 1024.0);
    else
        snprintf(buf, bufSize, "%8llu  B", size);
}

/* Tạo cây thư mục test / Create test directory tree */
void CreateTestTree() {
    CreateDirectoryA("find_test", NULL);
    CreateDirectoryA("find_test\\docs", NULL);
    CreateDirectoryA("find_test\\src", NULL);
    CreateDirectoryA("find_test\\src\\headers", NULL);
    CreateDirectoryA("find_test\\build", NULL);

    // Tạo các file test / Create test files
    const char* files[] = {
        "find_test\\readme.txt",
        "find_test\\notes.txt",
        "find_test\\docs\\guide.txt",
        "find_test\\docs\\api.md",
        "find_test\\docs\\report.pdf",
        "find_test\\src\\main.cpp",
        "find_test\\src\\utils.cpp",
        "find_test\\src\\headers\\utils.h",
        "find_test\\src\\headers\\config.h",
        "find_test\\build\\output.exe",
        "find_test\\build\\debug.log"
    };

    for (int i = 0; i < 11; i++) {
        HANDLE hFile = CreateFileA(files[i], GENERIC_WRITE, 0, NULL,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            char content[128];
            snprintf(content, sizeof(content), "Test content for %s\r\n", files[i]);
            DWORD written;
            WriteFile(hFile, content, (DWORD)strlen(content), &written, NULL);
            CloseHandle(hFile);
        }
    }

    // Đặt thuộc tính ẩn cho một file / Set hidden attribute on one file
    SetFileAttributesA("find_test\\build\\debug.log",
                       FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE);
}

/* Xóa cây thư mục test (đệ quy) / Delete test tree (recursive) */
void DeleteTestTree(const char* dirPath);

// ============================================================================
// PHẦN 1: Liệt kê file cơ bản với FindFirstFile / FindNextFile
// PART 1: Basic file listing with FindFirstFile / FindNextFile
// ============================================================================
void DemoBasicListing() {
    printf("=== PHAN 1: LIET KE FILE CO BAN / BASIC FILE LISTING ===\n\n");

    /*
     * FindFirstFileA() - Bắt đầu tìm kiếm file
     * FindFirstFileA() - Start a file search
     *
     * Tham số / Parameters:
     *   lpFileName    : Pattern tìm kiếm (hỗ trợ wildcard * và ?)
     *                   Search pattern (supports * and ? wildcards)
     *                   Ví dụ / Examples:
     *                     "C:\\dir\\*"       - Tất cả file / All files
     *                     "C:\\dir\\*.txt"   - Chỉ file .txt / Only .txt files
     *                     "C:\\dir\\test?.c" - test1.c, testA.c, etc.
     *   lpFindFileData : [out] Cấu trúc nhận thông tin file
     *                    [out] Structure to receive file information
     *
     * Trả về handle tìm kiếm hoặc INVALID_HANDLE_VALUE nếu thất bại
     * Returns search handle or INVALID_HANDLE_VALUE on failure
     *
     * CẤU TRÚC WIN32_FIND_DATA / WIN32_FIND_DATA STRUCTURE:
     *   dwFileAttributes  : Thuộc tính file (directory, hidden, etc.)
     *   ftCreationTime    : Thời gian tạo / Creation time
     *   ftLastAccessTime  : Thời gian truy cập cuối / Last access
     *   ftLastWriteTime   : Thời gian sửa cuối / Last modification
     *   nFileSizeHigh     : 32 bit cao của kích thước / High 32 bits of size
     *   nFileSizeLow      : 32 bit thấp của kích thước / Low 32 bits of size
     *   cFileName         : Tên file (tối đa MAX_PATH) / File name
     *   cAlternateFileName: Tên 8.3 ngắn / Short 8.3 name
     */

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA("find_test\\*", &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        PrintError("FindFirstFileA");
        return;
    }

    printf("  Noi dung thu muc / Directory contents: find_test\\\n\n");
    printf("  %-5s  %-16s  %-12s  %s\n", "Loai", "Ngay sua/Modified",
           "Kich thuoc", "Ten/Name");
    printf("  %-5s  %-16s  %-12s  %s\n", "Type", "Modified Date",
           "Size", "Name");
    printf("  %s\n", "-----  ----------------  ------------  --------------------");

    int fileCount = 0;
    int dirCount = 0;
    ULONGLONG totalSize = 0;

    do {
        /*
         * Bỏ qua thư mục "." và ".."
         * Skip "." (current) and ".." (parent) directory entries
         *
         * "." đại diện cho thư mục hiện tại / represents current directory
         * ".." đại diện cho thư mục cha / represents parent directory
         * Mọi thư mục trên NTFS đều có 2 entry này
         * Every directory on NTFS has these 2 entries
         */
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        BOOL isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        BOOL isHidden = (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN);

        char timeStr[64];
        FileTimeToStr(&findData.ftLastWriteTime, timeStr, sizeof(timeStr));

        ULONGLONG fileSize = ((ULONGLONG)findData.nFileSizeHigh << 32)
                             | findData.nFileSizeLow;

        if (isDir) {
            printf("  <DIR>  %s              -  %s%s\n",
                   timeStr, findData.cFileName,
                   isHidden ? " [AN/HIDDEN]" : "");
            dirCount++;
        } else {
            char sizeStr[32];
            FormatSize(fileSize, sizeStr, sizeof(sizeStr));
            printf("  FILE   %s  %s  %s%s\n",
                   timeStr, sizeStr, findData.cFileName,
                   isHidden ? " [AN/HIDDEN]" : "");
            fileCount++;
            totalSize += fileSize;
        }

        /*
         * FindNextFileA() - Tìm file tiếp theo
         * FindNextFileA() - Find the next file
         *
         * Trả về TRUE nếu tìm thấy, FALSE nếu không còn file
         * Returns TRUE if found, FALSE if no more files
         * Khi không còn file, GetLastError() = ERROR_NO_MORE_FILES
         * When no more files, GetLastError() = ERROR_NO_MORE_FILES
         */
    } while (FindNextFileA(hFind, &findData));

    /*
     * FindClose() - Đóng handle tìm kiếm
     * FindClose() - Close the search handle
     *
     * QUAN TRỌNG: Luôn gọi FindClose() sau khi dùng xong
     * IMPORTANT: Always call FindClose() when done
     * Quên gọi sẽ gây rò rỉ handle tìm kiếm
     * Forgetting causes search handle leaks
     */
    FindClose(hFind);

    char totalStr[32];
    FormatSize(totalSize, totalStr, sizeof(totalStr));
    printf("  %s\n", "---------------------------------------------");
    printf("  Tong: %d file, %d thu muc, %s\n",
           fileCount, dirCount, totalStr);
    printf("  Total: %d files, %d directories, %s\n\n",
           fileCount, dirCount, totalStr);
}

// ============================================================================
// PHẦN 2: Tìm kiếm với wildcard / pattern
// PART 2: Search with wildcard / pattern
// ============================================================================
void DemoWildcardSearch() {
    printf("=== PHAN 2: TIM KIEM VOI WILDCARD / WILDCARD SEARCH ===\n\n");

    /*
     * Wildcard trong Windows File Search:
     * Wildcards in Windows File Search:
     *   * : Khớp 0 hoặc nhiều ký tự bất kỳ / Match 0 or more any characters
     *   ? : Khớp đúng 1 ký tự bất kỳ / Match exactly 1 any character
     *
     * Ví dụ / Examples:
     *   *.txt     -> readme.txt, notes.txt
     *   test?.c   -> test1.c, testA.c (nhưng không test12.c)
     *   *.*       -> Tất cả file có extension
     *   *         -> Tất cả file và thư mục
     */

    const char* patterns[] = {
        "find_test\\*.txt",
        "find_test\\src\\*.cpp",
        "find_test\\src\\headers\\*.h",
        "find_test\\docs\\*.*"
    };
    const char* descriptions[] = {
        "*.txt trong find_test/",
        "*.cpp trong find_test/src/",
        "*.h trong find_test/src/headers/",
        "*.* trong find_test/docs/"
    };

    for (int p = 0; p < 4; p++) {
        printf("  Pattern: %s\n", patterns[p]);
        printf("  (%s)\n", descriptions[p]);

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(patterns[p], &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                printf("    -> Khong tim thay file nao / No files found.\n\n");
            } else {
                PrintError("FindFirstFileA");
            }
            continue;
        }

        int count = 0;
        do {
            if (strcmp(findData.cFileName, ".") == 0 ||
                strcmp(findData.cFileName, "..") == 0) continue;

            count++;
            ULONGLONG size = ((ULONGLONG)findData.nFileSizeHigh << 32) |
                             findData.nFileSizeLow;
            printf("    [%d] %s (%llu bytes)\n", count,
                   findData.cFileName, size);
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
        printf("    -> Tim thay / Found: %d file(s)\n\n", count);
    }
}

// ============================================================================
// PHẦN 3: Duyệt cây thư mục đệ quy (list_tree)
// PART 3: Recursive directory traversal (list_tree)
// ============================================================================

/*
 * Hàm liệt kê đệ quy toàn bộ cây thư mục
 * Recursive function to list entire directory tree
 *
 * Đây là cốt lõi của lệnh list_tree trong Tiny Shell
 * This is the core of the list_tree command in Tiny Shell
 *
 * Thuật toán / Algorithm:
 *   1. FindFirstFile(dir\\*) để lấy tất cả entry
 *      FindFirstFile(dir\\*) to get all entries
 *   2. Với mỗi entry:
 *      For each entry:
 *      a. Nếu là file -> in thông tin / If file -> print info
 *      b. Nếu là thư mục -> in tên + gọi đệ quy
 *         If directory -> print name + recurse
 *   3. FindClose() để giải phóng / FindClose() to release
 *
 * Tham số / Parameters:
 *   dirPath : Đường dẫn thư mục / Directory path
 *   indent  : Mức thụt đầu dòng (cho cây) / Indentation level (for tree)
 *   stats   : Con trỏ tới bộ đếm thống kê / Pointer to statistics counters
 */

struct TreeStats {
    int totalFiles;
    int totalDirs;
    ULONGLONG totalSize;
};

void ListTree(const char* dirPath, int indent, TreeStats* stats) {
    char searchPattern[MAX_PATH];
    snprintf(searchPattern, sizeof(searchPattern), "%s\\*", dirPath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPattern, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        // Bỏ qua . và .. / Skip . and ..
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        // In thụt đầu dòng (tạo hình cây) / Print indentation (tree shape)
        for (int i = 0; i < indent; i++) printf("  ");

        BOOL isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

        if (isDir) {
            /*
             * Ký hiệu cây thư mục / Directory tree symbols:
             * Dùng ký tự ASCII đơn giản thay vì Unicode để tương thích
             * Using simple ASCII instead of Unicode for compatibility
             */
            printf("|-- [DIR] %s\\\n", findData.cFileName);
            stats->totalDirs++;

            // Xây dựng đường dẫn đầy đủ cho đệ quy
            // Build full path for recursion
            char subDirPath[MAX_PATH];
            snprintf(subDirPath, sizeof(subDirPath), "%s\\%s",
                     dirPath, findData.cFileName);

            // GỌI ĐỆ QUY / RECURSE
            ListTree(subDirPath, indent + 1, stats);

        } else {
            ULONGLONG fileSize = ((ULONGLONG)findData.nFileSizeHigh << 32) |
                                 findData.nFileSizeLow;
            char sizeStr[32];
            FormatSize(fileSize, sizeStr, sizeof(sizeStr));

            printf("|-- %s (%s)\n", findData.cFileName, sizeStr);
            stats->totalFiles++;
            stats->totalSize += fileSize;
        }

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

void DemoRecursiveListing() {
    printf("=== PHAN 3: DUYET CAY THU MUC DE QUY / RECURSIVE TREE ===\n\n");

    printf("  Cay thu muc / Directory tree: find_test\\\n\n");
    printf("  find_test\\\n");

    TreeStats stats = {0, 0, 0};
    ListTree("find_test", 1, &stats);

    char totalStr[32];
    FormatSize(stats.totalSize, totalStr, sizeof(totalStr));
    printf("\n  --- Thong ke / Statistics ---\n");
    printf("  Tong so file / Total files       : %d\n", stats.totalFiles);
    printf("  Tong so thu muc / Total dirs     : %d\n", stats.totalDirs);
    printf("  Tong kich thuoc / Total size     : %s\n\n", totalStr);
}

// ============================================================================
// PHẦN 4: Đếm file theo loại (extension)
// PART 4: Count files by type (extension)
// ============================================================================
void CountFilesByExtension(const char* dirPath) {
    /*
     * Ứng dụng thực tế: phân tích thư mục dự án
     * Practical application: analyze project directory
     */

    // Cấu trúc đếm theo extension / Count by extension structure
    struct ExtCount {
        char ext[16];
        int count;
        ULONGLONG totalSize;
    };

    ExtCount extCounts[20];
    int extTypeCount = 0;

    // Duyệt đệ quy tương tự ListTree nhưng đơn giản hơn
    // Simplified recursive traversal similar to ListTree

    // Dùng stack giả lập đệ quy để minh họa cách tiếp cận khác
    // Use stack to simulate recursion (alternative approach)
    char dirStack[50][MAX_PATH];
    int stackTop = 0;
    strncpy(dirStack[stackTop++], dirPath, MAX_PATH);

    while (stackTop > 0) {
        char currentDir[MAX_PATH];
        strncpy(currentDir, dirStack[--stackTop], MAX_PATH);

        char searchPath[MAX_PATH];
        snprintf(searchPath, sizeof(searchPath), "%s\\*", currentDir);

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath, &findData);
        if (hFind == INVALID_HANDLE_VALUE) continue;

        do {
            if (strcmp(findData.cFileName, ".") == 0 ||
                strcmp(findData.cFileName, "..") == 0) continue;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (stackTop < 50) {
                    snprintf(dirStack[stackTop++], MAX_PATH, "%s\\%s",
                             currentDir, findData.cFileName);
                }
            } else {
                // Tìm extension / Find extension
                const char* ext = strrchr(findData.cFileName, '.');
                if (!ext) ext = "(none)";

                ULONGLONG size = ((ULONGLONG)findData.nFileSizeHigh << 32) |
                                 findData.nFileSizeLow;

                // Tìm hoặc thêm extension / Find or add extension
                int found = -1;
                for (int i = 0; i < extTypeCount; i++) {
                    if (_stricmp(extCounts[i].ext, ext) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found >= 0) {
                    extCounts[found].count++;
                    extCounts[found].totalSize += size;
                } else if (extTypeCount < 20) {
                    strncpy(extCounts[extTypeCount].ext, ext, 15);
                    extCounts[extTypeCount].ext[15] = '\0';
                    extCounts[extTypeCount].count = 1;
                    extCounts[extTypeCount].totalSize = size;
                    extTypeCount++;
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }

    // In kết quả / Print results
    printf("  %-10s  %-8s  %s\n", "Extension", "So luong", "Tong kich thuoc");
    printf("  %-10s  %-8s  %s\n", "Extension", "Count", "Total Size");
    printf("  %s\n", "----------  --------  ---------------");
    for (int i = 0; i < extTypeCount; i++) {
        char sizeStr[32];
        FormatSize(extCounts[i].totalSize, sizeStr, sizeof(sizeStr));
        printf("  %-10s  %5d     %s\n",
               extCounts[i].ext, extCounts[i].count, sizeStr);
    }
    printf("\n");
}

void DemoCountByExtension() {
    printf("=== PHAN 4: DEM FILE THEO LOAI / COUNT BY EXTENSION ===\n\n");
    CountFilesByExtension("find_test");
}

// ============================================================================
// Hàm xóa cây thư mục test
// Delete test directory tree
// ============================================================================
void DeleteTestTree(const char* dirPath) {
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", dirPath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0) continue;

        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", dirPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            DeleteTestTree(fullPath); // Đệ quy / Recurse
            RemoveDirectoryA(fullPath);
        } else {
            // Xóa thuộc tính đặc biệt trước khi xóa file
            // Remove special attributes before deleting
            SetFileAttributesA(fullPath, FILE_ATTRIBUTE_NORMAL);
            DeleteFileA(fullPath);
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH / MAIN PROGRAM
// ============================================================================
int main() {
    printf("************************************************************\n");
    printf("* BAI TAP 05: TIM KIEM FILE                                *\n");
    printf("* Exercise 05: Find Files                                  *\n");
    printf("************************************************************\n\n");

    // Tạo cây thư mục test / Create test directory tree
    printf("[SETUP] Dang tao cay thu muc test...\n");
    printf("        Creating test directory tree...\n\n");
    CreateTestTree();

    DemoBasicListing();
    DemoWildcardSearch();
    DemoRecursiveListing();
    DemoCountByExtension();

    // Dọn dẹp / Cleanup
    printf("[CLEANUP] Xoa cay thu muc test / Deleting test tree...\n");
    DeleteTestTree("find_test");
    RemoveDirectoryA("find_test");
    printf("[OK] Da don dep / Cleaned up.\n\n");

    printf("************************************************************\n");
    printf("* HOAN THANH! / DONE!                                      *\n");
    printf("************************************************************\n");

    return 0;
}
