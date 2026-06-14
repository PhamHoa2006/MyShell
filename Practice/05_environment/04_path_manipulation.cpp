/*
 * ============================================================================
 * Exercise 04: PATH Manipulation (Thao Tác Biến PATH)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate reading, parsing, modifying, and managing the PATH variable.
 *   This directly mirrors the add_path/remove_path/is_in_path commands
 *   in the Tiny Shell's EnvironmentManager.
 *   Minh họa đọc, phân tích, thay đổi và quản lý biến PATH.
 *   Bài này phản ánh trực tiếp các lệnh add_path/remove_path/is_in_path
 *   trong EnvironmentManager của Tiny Shell.
 *
 *   The PATH variable contains a semicolon-separated list of directories.
 *   When you type a command, Windows searches these directories in order.
 *   Biến PATH chứa danh sách thư mục phân cách bằng dấu chấm phẩy.
 *   Khi bạn gõ lệnh, Windows tìm kiếm các thư mục này theo thứ tự.
 *
 * Key APIs (Các API chính):
 *   - GetEnvironmentVariableA() : Read PATH (Đọc PATH)
 *   - SetEnvironmentVariableA() : Update PATH (Cập nhật PATH)
 *
 * Key Operations (Các thao tác chính):
 *   - list_path    : Show all directories in PATH
 *   - is_in_path   : Check if a directory is in PATH
 *   - add_path     : Add a directory to PATH
 *   - remove_path  : Remove a directory from PATH
 *
 * Compile (Biên dịch):
 *   g++ 04_path_manipulation.cpp -o 04_path_manipulation.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Maximum PATH length (can be up to 32767 on Windows)
// Độ dài PATH tối đa (có thể lên đến 32767 trên Windows)
#define MAX_PATH_LEN 32768

// ============================================================================
// Helper: Read the current PATH into a buffer
// Trợ giúp: Đọc PATH hiện tại vào buffer
// ============================================================================
bool GetCurrentPath(char* buffer, DWORD bufSize) {
    DWORD result = GetEnvironmentVariableA("PATH", buffer, bufSize);
    if (result == 0) {
        fprintf(stderr, "[ERROR] Cannot read PATH: %lu\n", GetLastError());
        return false;
    }
    if (result >= bufSize) {
        fprintf(stderr, "[ERROR] PATH too long! Need %lu chars.\n", result);
        return false;
    }
    return true;
}

// ============================================================================
// Operation 1: list_path - Show all directories in PATH
// Thao tác 1: list_path - Hiển thị tất cả thư mục trong PATH
// ============================================================================
void ListPath() {
    printf("=== list_path: All Directories in PATH ===\n");
    printf("=== list_path: Tat Ca Thu Muc Trong PATH ===\n\n");

    char pathBuf[MAX_PATH_LEN];
    if (!GetCurrentPath(pathBuf, sizeof(pathBuf))) return;

    // PATH uses semicolons as delimiters on Windows
    // PATH dùng dấu chấm phẩy làm dấu phân cách trên Windows
    printf("PATH directories (Thu muc PATH):\n\n");

    // Make a copy for strtok (strtok modifies the string)
    // Tạo bản sao cho strtok (strtok thay đổi chuỗi)
    char* pathCopy = _strdup(pathBuf);
    if (!pathCopy) {
        fprintf(stderr, "[ERROR] Memory allocation failed.\n");
        return;
    }

    int dirCount = 0;
    int existCount = 0;

    // strtok splits string by delimiter (Phân tách chuỗi theo dấu phân cách)
    char* token = strtok(pathCopy, ";");
    while (token != NULL) {
        dirCount++;

        // Check if directory actually exists
        // Kiểm tra xem thư mục có thực sự tồn tại không
        DWORD attr = GetFileAttributesA(token);
        bool exists = (attr != INVALID_FILE_ATTRIBUTES &&
                      (attr & FILE_ATTRIBUTE_DIRECTORY));
        if (exists) existCount++;

        printf("  [%2d] %s%s\n", dirCount, token,
               exists ? "" : "  [NOT FOUND / KHONG TIM THAY]");

        token = strtok(NULL, ";");
    }

    printf("\nTotal: %d directories (%d exist, %d missing)\n",
           dirCount, existCount, dirCount - existCount);
    printf("Tong cong: %d thu muc (%d ton tai, %d thieu)\n\n",
           dirCount, existCount, dirCount - existCount);

    free(pathCopy);
}

// ============================================================================
// Operation 2: is_in_path - Check if a directory is in PATH
// Thao tác 2: is_in_path - Kiểm tra thư mục có trong PATH không
// ============================================================================
bool IsInPath(const char* directory, bool verbose) {
    char pathBuf[MAX_PATH_LEN];
    if (!GetCurrentPath(pathBuf, sizeof(pathBuf))) return false;

    // Normalize the search directory: remove trailing backslash
    // Chuẩn hóa thư mục tìm kiếm: xóa dấu gạch chéo cuối
    char normalizedDir[MAX_PATH];
    strncpy(normalizedDir, directory, sizeof(normalizedDir) - 1);
    normalizedDir[sizeof(normalizedDir) - 1] = '\0';

    size_t len = strlen(normalizedDir);
    if (len > 0 && (normalizedDir[len - 1] == '\\' || normalizedDir[len - 1] == '/')) {
        normalizedDir[len - 1] = '\0';
    }

    // Search through PATH entries (case-insensitive on Windows)
    // Tìm kiếm qua các mục PATH (không phân biệt hoa thường trên Windows)
    char* pathCopy = _strdup(pathBuf);
    if (!pathCopy) return false;

    bool found = false;
    int position = 0;
    char* token = strtok(pathCopy, ";");

    while (token != NULL) {
        position++;

        // Normalize this PATH entry too (Chuẩn hóa mục PATH này)
        char normalizedToken[MAX_PATH];
        strncpy(normalizedToken, token, sizeof(normalizedToken) - 1);
        normalizedToken[sizeof(normalizedToken) - 1] = '\0';

        size_t tokenLen = strlen(normalizedToken);
        if (tokenLen > 0 && (normalizedToken[tokenLen - 1] == '\\' ||
                             normalizedToken[tokenLen - 1] == '/')) {
            normalizedToken[tokenLen - 1] = '\0';
        }

        // Case-insensitive comparison (So sánh không phân biệt hoa thường)
        if (_stricmp(normalizedDir, normalizedToken) == 0) {
            found = true;
            if (verbose) {
                printf("  FOUND at position %d: \"%s\"\n", position, token);
                printf("  TIM THAY tai vi tri %d: \"%s\"\n", position, token);
            }
            break; // Could continue to find duplicates (Có thể tiếp tục tìm bản sao)
        }

        token = strtok(NULL, ";");
    }

    if (!found && verbose) {
        printf("  NOT FOUND in PATH: \"%s\"\n", directory);
        printf("  KHONG TIM THAY trong PATH: \"%s\"\n", directory);
    }

    free(pathCopy);
    return found;
}

// ============================================================================
// Operation 3: add_path - Add a directory to PATH
// Thao tác 3: add_path - Thêm thư mục vào PATH
// ============================================================================
bool AddPath(const char* directory, bool prepend) {
    // Check if already in PATH (Kiểm tra xem đã có trong PATH chưa)
    if (IsInPath(directory, false)) {
        printf("  SKIPPED: \"%s\" is already in PATH.\n", directory);
        printf("  BO QUA: \"%s\" da co trong PATH.\n", directory);
        return false;
    }

    char pathBuf[MAX_PATH_LEN];
    if (!GetCurrentPath(pathBuf, sizeof(pathBuf))) return false;

    // Build new PATH
    // Xây dựng PATH mới
    char newPath[MAX_PATH_LEN];

    if (prepend) {
        // Add to the beginning (highest priority)
        // Thêm vào đầu (ưu tiên cao nhất)
        snprintf(newPath, sizeof(newPath), "%s;%s", directory, pathBuf);
    } else {
        // Add to the end (lowest priority)
        // Thêm vào cuối (ưu tiên thấp nhất)
        snprintf(newPath, sizeof(newPath), "%s;%s", pathBuf, directory);
    }

    // Update PATH with SetEnvironmentVariable
    // Cập nhật PATH bằng SetEnvironmentVariable
    if (!SetEnvironmentVariableA("PATH", newPath)) {
        fprintf(stderr, "[ERROR] Failed to update PATH: %lu\n", GetLastError());
        return false;
    }

    printf("  ADDED: \"%s\" %s PATH.\n",
           directory, prepend ? "to beginning of" : "to end of");
    printf("  DA THEM: \"%s\" vao %s PATH.\n",
           directory, prepend ? "dau" : "cuoi");
    return true;
}

// ============================================================================
// Operation 4: remove_path - Remove a directory from PATH
// Thao tác 4: remove_path - Xóa thư mục khỏi PATH
// ============================================================================
bool RemovePath(const char* directory) {
    char pathBuf[MAX_PATH_LEN];
    if (!GetCurrentPath(pathBuf, sizeof(pathBuf))) return false;

    // Normalize the directory to remove
    // Chuẩn hóa thư mục cần xóa
    char normalizedDir[MAX_PATH];
    strncpy(normalizedDir, directory, sizeof(normalizedDir) - 1);
    normalizedDir[sizeof(normalizedDir) - 1] = '\0';
    size_t ndLen = strlen(normalizedDir);
    if (ndLen > 0 && (normalizedDir[ndLen - 1] == '\\' || normalizedDir[ndLen - 1] == '/')) {
        normalizedDir[ndLen - 1] = '\0';
    }

    // Rebuild PATH without the specified directory
    // Xây dựng lại PATH mà không có thư mục được chỉ định
    char newPath[MAX_PATH_LEN] = {0};
    char* pathCopy = _strdup(pathBuf);
    if (!pathCopy) return false;

    bool found = false;
    int removedCount = 0;
    bool firstEntry = true;

    char* token = strtok(pathCopy, ";");
    while (token != NULL) {
        // Normalize this entry (Chuẩn hóa mục này)
        char normalizedToken[MAX_PATH];
        strncpy(normalizedToken, token, sizeof(normalizedToken) - 1);
        normalizedToken[sizeof(normalizedToken) - 1] = '\0';
        size_t tLen = strlen(normalizedToken);
        if (tLen > 0 && (normalizedToken[tLen - 1] == '\\' || normalizedToken[tLen - 1] == '/')) {
            normalizedToken[tLen - 1] = '\0';
        }

        if (_stricmp(normalizedDir, normalizedToken) == 0) {
            // Skip this entry (match found) (Bỏ qua mục này (tìm thấy))
            found = true;
            removedCount++;
        } else {
            // Keep this entry (Giữ mục này)
            if (!firstEntry) {
                strcat(newPath, ";");
            }
            strcat(newPath, token);
            firstEntry = false;
        }

        token = strtok(NULL, ";");
    }

    free(pathCopy);

    if (!found) {
        printf("  NOT FOUND: \"%s\" is not in PATH.\n", directory);
        printf("  KHONG TIM THAY: \"%s\" khong co trong PATH.\n", directory);
        return false;
    }

    // Update PATH (Cập nhật PATH)
    if (!SetEnvironmentVariableA("PATH", newPath)) {
        fprintf(stderr, "[ERROR] Failed to update PATH: %lu\n", GetLastError());
        return false;
    }

    printf("  REMOVED: \"%s\" from PATH (%d occurrence(s)).\n",
           directory, removedCount);
    printf("  DA XOA: \"%s\" khoi PATH (%d lan xuat hien).\n",
           directory, removedCount);
    return true;
}

// ============================================================================
// Demo 1: Show current PATH
// Minh họa 1: Hiển thị PATH hiện tại
// ============================================================================
void DemoShowPath() {
    printf("=== Demo 1: Current PATH ===\n");
    printf("=== Minh hoa 1: PATH Hien Tai ===\n\n");
    ListPath();
}

// ============================================================================
// Demo 2: Check if specific directories are in PATH
// Minh họa 2: Kiểm tra xem thư mục cụ thể có trong PATH không
// ============================================================================
void DemoIsInPath() {
    printf("=== Demo 2: is_in_path - Check PATH Membership ===\n");
    printf("=== Minh hoa 2: is_in_path - Kiem Tra Thanh Vien PATH ===\n\n");

    const char* testPaths[] = {
        "C:\\Windows\\System32",
        "C:\\Windows",
        "C:\\NonExistent\\Fake\\Path",
        "C:\\Windows\\System32\\", // With trailing backslash (Có dấu gạch cuối)
    };
    int numTests = sizeof(testPaths) / sizeof(testPaths[0]);

    for (int i = 0; i < numTests; i++) {
        printf("Checking: \"%s\"\n", testPaths[i]);
        printf("Kiem tra: \"%s\"\n", testPaths[i]);
        IsInPath(testPaths[i], true);
        printf("\n");
    }
}

// ============================================================================
// Demo 3: Add and remove paths
// Minh họa 3: Thêm và xóa đường dẫn
// ============================================================================
void DemoAddRemove() {
    printf("=== Demo 3: add_path & remove_path ===\n");
    printf("=== Minh hoa 3: add_path & remove_path ===\n\n");

    const char* testDir1 = "C:\\MyCustomTools";
    const char* testDir2 = "D:\\Projects\\bin";

    // -----------------------------------------------------------------------
    // Step 1: Add paths
    // Bước 1: Thêm đường dẫn
    // -----------------------------------------------------------------------
    printf("--- Adding paths (Them duong dan) ---\n\n");

    printf("add_path(\"%s\", prepend=true):\n", testDir1);
    AddPath(testDir1, true);  // Add to beginning (Thêm vào đầu)
    printf("\n");

    printf("add_path(\"%s\", prepend=false):\n", testDir2);
    AddPath(testDir2, false); // Add to end (Thêm vào cuối)
    printf("\n");

    // -----------------------------------------------------------------------
    // Step 2: Verify additions
    // Bước 2: Xác nhận đã thêm
    // -----------------------------------------------------------------------
    printf("--- Verifying additions (Xac nhan da them) ---\n\n");

    printf("is_in_path(\"%s\"): ", testDir1);
    IsInPath(testDir1, true);
    printf("\n");

    printf("is_in_path(\"%s\"): ", testDir2);
    IsInPath(testDir2, true);
    printf("\n");

    // -----------------------------------------------------------------------
    // Step 3: Try adding a duplicate
    // Bước 3: Thử thêm bản sao
    // -----------------------------------------------------------------------
    printf("--- Adding duplicate (Them ban sao) ---\n\n");
    printf("add_path(\"%s\") again:\n", testDir1);
    AddPath(testDir1, true);
    printf("\n");

    // -----------------------------------------------------------------------
    // Step 4: Remove paths
    // Bước 4: Xóa đường dẫn
    // -----------------------------------------------------------------------
    printf("--- Removing paths (Xoa duong dan) ---\n\n");

    printf("remove_path(\"%s\"):\n", testDir1);
    RemovePath(testDir1);
    printf("\n");

    printf("remove_path(\"%s\"):\n", testDir2);
    RemovePath(testDir2);
    printf("\n");

    // -----------------------------------------------------------------------
    // Step 5: Verify removals
    // Bước 5: Xác nhận đã xóa
    // -----------------------------------------------------------------------
    printf("--- Verifying removals (Xac nhan da xoa) ---\n\n");

    printf("is_in_path(\"%s\"): ", testDir1);
    IsInPath(testDir1, true);
    printf("\n");

    printf("is_in_path(\"%s\"): ", testDir2);
    IsInPath(testDir2, true);
    printf("\n");

    // -----------------------------------------------------------------------
    // Step 6: Try removing non-existent path
    // Bước 6: Thử xóa đường dẫn không tồn tại
    // -----------------------------------------------------------------------
    printf("--- Removing non-existent path ---\n\n");
    printf("remove_path(\"C:\\\\Nonexistent\"):\n");
    RemovePath("C:\\Nonexistent");
    printf("\n");
}

// ============================================================================
// Demo 4: Find executables in PATH (like 'which' or 'where' command)
// Minh họa 4: Tìm file thực thi trong PATH (giống lệnh 'which' hoặc 'where')
// ============================================================================
void DemoFindExecutable(const char* exeName) {
    printf("--- Finding \"%s\" in PATH ---\n", exeName);
    printf("--- Tim \"%s\" trong PATH ---\n\n", exeName);

    char pathBuf[MAX_PATH_LEN];
    if (!GetCurrentPath(pathBuf, sizeof(pathBuf))) return;

    char* pathCopy = _strdup(pathBuf);
    if (!pathCopy) return;

    int found = 0;
    char* token = strtok(pathCopy, ";");

    while (token != NULL) {
        // Build full path: directory + \ + executable name
        // Xây dựng đường dẫn đầy đủ: thư mục + \ + tên file
        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", token, exeName);

        // Check if file exists (Kiểm tra file có tồn tại không)
        DWORD attr = GetFileAttributesA(fullPath);
        if (attr != INVALID_FILE_ATTRIBUTES &&
            !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
            found++;
            printf("  [%d] FOUND: %s\n", found, fullPath);
        }

        token = strtok(NULL, ";");
    }

    if (found == 0) {
        printf("  NOT FOUND in any PATH directory.\n");
        printf("  KHONG TIM THAY trong bat ky thu muc PATH nao.\n");
    } else {
        printf("\n  Total: %d location(s) found.\n", found);
        printf("  Tong cong: %d vi tri tim thay.\n", found);
    }
    printf("\n");

    free(pathCopy);
}

void DemoFindExecutables() {
    printf("=== Demo 4: Find Executables in PATH (Tim File Thuc Thi) ===\n\n");

    // Search for common executables (Tìm kiếm các file thực thi phổ biến)
    DemoFindExecutable("cmd.exe");
    DemoFindExecutable("notepad.exe");
    DemoFindExecutable("python.exe");
    DemoFindExecutable("gcc.exe");
    DemoFindExecutable("nonexistent_program.exe");
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main() {
    printf("============================================================\n");
    printf("  PATH Manipulation Exercise\n");
    printf("  Bai Tap Thao Tac PATH\n");
    printf("  (Mirrors Tiny Shell EnvironmentManager)\n");
    printf("  (Phan anh EnvironmentManager cua Tiny Shell)\n");
    printf("============================================================\n\n");

    DemoShowPath();
    DemoIsInPath();
    DemoAddRemove();
    DemoFindExecutables();

    printf("============================================================\n");
    printf("  Summary of PATH Operations (Tom tat thao tac PATH):\n");
    printf("  - list_path    : Show all PATH directories\n");
    printf("  - is_in_path   : Check if dir is in PATH\n");
    printf("  - add_path     : Add dir to PATH (prepend/append)\n");
    printf("  - remove_path  : Remove dir from PATH\n");
    printf("  - find_exe     : Find executable in PATH (like 'where')\n");
    printf("============================================================\n");
    printf("  Exercise Complete! (Bai tap hoan tat!)\n");
    printf("============================================================\n");

    return 0;
}
