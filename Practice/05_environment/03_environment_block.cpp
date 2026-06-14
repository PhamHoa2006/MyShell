/*
 * ============================================================================
 * Exercise 03: Environment Block (Khối Biến Môi Trường)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate GetEnvironmentStrings to retrieve the entire environment
 *   block of the current process. Parse and display all variables.
 *   Minh họa GetEnvironmentStrings để lấy toàn bộ khối biến môi trường
 *   của tiến trình hiện tại. Phân tích và hiển thị tất cả các biến.
 *
 *   The environment block is a contiguous block of memory containing
 *   all NAME=VALUE pairs, each null-terminated, with an extra null at end.
 *   Khối môi trường là vùng nhớ liên tục chứa tất cả cặp NAME=VALUE,
 *   mỗi cặp kết thúc bằng null, với thêm một null ở cuối.
 *
 *   Layout: NAME1=VALUE1\0NAME2=VALUE2\0...\0\0
 *
 * Key APIs (Các API chính):
 *   - GetEnvironmentStringsA() : Get environment block (Lấy khối môi trường)
 *   - FreeEnvironmentStringsA(): Free the block (Giải phóng khối)
 *
 * Compile (Biên dịch):
 *   g++ 03_environment_block.cpp -o 03_environment_block.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ============================================================================
// Demo 1: Get and display the raw environment block
// Minh họa 1: Lấy và hiển thị khối môi trường thô
// ============================================================================
void DemoRawBlock() {
    printf("=== Demo 1: Raw Environment Block ===\n");
    printf("=== Minh hoa 1: Khoi Moi Truong Tho ===\n\n");

    // -----------------------------------------------------------------------
    // GetEnvironmentStrings returns a pointer to the environment block.
    // GetEnvironmentStrings trả về con trỏ đến khối môi trường.
    //
    // The block format is: VAR1=VALUE1\0VAR2=VALUE2\0...\0\0
    // Double null (\0\0) marks the end of the block.
    // Định dạng khối: VAR1=VALUE1\0VAR2=VALUE2\0...\0\0
    // Null kép (\0\0) đánh dấu cuối khối.
    // -----------------------------------------------------------------------
    LPCH envBlock = GetEnvironmentStringsA();

    if (envBlock == NULL) {
        fprintf(stderr, "[ERROR] GetEnvironmentStrings failed: %lu\n",
                GetLastError());
        return;
    }

    printf("Environment block retrieved at address: %p\n", envBlock);
    printf("Khoi moi truong lay duoc tai dia chi: %p\n\n", envBlock);

    // Calculate total block size (Tính tổng kích thước khối)
    LPCH p = envBlock;
    int totalVars = 0;
    size_t totalSize = 0;

    while (*p != '\0') {
        size_t len = strlen(p);
        totalSize += len + 1; // +1 for null terminator
        totalVars++;
        p += len + 1; // Move to next variable (Di chuyển đến biến tiếp theo)
    }
    totalSize++; // Final null terminator (Ký tự null cuối cùng)

    printf("Total variables: %d\n", totalVars);
    printf("Tong so bien: %d\n", totalVars);
    printf("Total block size: %zu bytes\n\n", totalSize);
    printf("Tong kich thuoc khoi: %zu bytes\n\n", totalSize);

    // -----------------------------------------------------------------------
    // IMPORTANT: Free the environment block when done!
    // QUAN TRỌNG: Giải phóng khối môi trường khi xong!
    // -----------------------------------------------------------------------
    FreeEnvironmentStringsA(envBlock);
    printf("Environment block freed. (Khoi moi truong da giai phong.)\n\n");
}

// ============================================================================
// Demo 2: Parse and display all environment variables
// Minh họa 2: Phân tích và hiển thị tất cả biến môi trường
// ============================================================================
void DemoParseAll() {
    printf("=== Demo 2: Parse All Environment Variables ===\n");
    printf("=== Minh hoa 2: Phan Tich Tat Ca Bien Moi Truong ===\n\n");

    LPCH envBlock = GetEnvironmentStringsA();
    if (envBlock == NULL) {
        fprintf(stderr, "[ERROR] GetEnvironmentStrings failed.\n");
        return;
    }

    // Iterate through the block, parsing each NAME=VALUE pair
    // Duyệt qua khối, phân tích từng cặp NAME=VALUE
    LPCH current = envBlock;
    int index = 0;

    printf("+-----+-----------------------------+-------------------------------+\n");
    printf("| #   | Name (Ten)                  | Value (Gia tri)               |\n");
    printf("+-----+-----------------------------+-------------------------------+\n");

    while (*current != '\0') {
        index++;

        // Find the '=' separator (Tìm dấu '=' phân cách)
        // Note: Some entries start with '=' (hidden system vars like =C:=C:\)
        // Ghi chú: Một số mục bắt đầu bằng '=' (biến hệ thống ẩn như =C:=C:\)
        char* equalsSign = NULL;

        if (current[0] == '=') {
            // Skip first '=' and find the real separator
            // Bỏ qua '=' đầu tiên và tìm dấu phân cách thực
            equalsSign = strchr(current + 1, '=');
        } else {
            equalsSign = strchr(current, '=');
        }

        if (equalsSign != NULL) {
            // Extract name and value (Trích xuất tên và giá trị)
            size_t nameLen = equalsSign - current;
            char name[256] = {0};
            char value[256] = {0};

            // Copy name (Sao chép tên)
            if (nameLen >= sizeof(name)) nameLen = sizeof(name) - 1;
            strncpy(name, current, nameLen);

            // Copy value, truncate if too long (Sao chép giá trị, cắt nếu quá dài)
            const char* val = equalsSign + 1;
            strncpy(value, val, sizeof(value) - 1);
            if (strlen(val) >= sizeof(value)) {
                value[sizeof(value) - 4] = '.';
                value[sizeof(value) - 3] = '.';
                value[sizeof(value) - 2] = '.';
            }

            // Truncate name for display (Cắt ngắn tên để hiển thị)
            if (strlen(name) > 27) {
                name[24] = '.';
                name[25] = '.';
                name[26] = '.';
                name[27] = '\0';
            }
            if (strlen(value) > 29) {
                value[26] = '.';
                value[27] = '.';
                value[28] = '.';
                value[29] = '\0';
            }

            printf("| %-3d | %-27s | %-29s |\n", index, name, value);
        } else {
            // No '=' found (unusual) (Không tìm thấy '=' (bất thường))
            printf("| %-3d | (no separator) %-12s |                               |\n",
                   index, current);
        }

        // Move to next string in the block
        // Di chuyển đến chuỗi tiếp theo trong khối
        current += strlen(current) + 1;
    }

    printf("+-----+-----------------------------+-------------------------------+\n");
    printf("Total: %d environment variables\n", index);
    printf("Tong cong: %d bien moi truong\n\n", index);

    FreeEnvironmentStringsA(envBlock);
}

// ============================================================================
// Demo 3: Search for variables by pattern
// Minh họa 3: Tìm kiếm biến theo mẫu
// ============================================================================
void DemoSearchVars(const char* searchPattern) {
    printf("=== Demo 3: Search Variables (Tim Kiem Bien) ===\n");
    printf("=== Searching for pattern: \"%s\" ===\n\n", searchPattern);

    LPCH envBlock = GetEnvironmentStringsA();
    if (envBlock == NULL) return;

    LPCH current = envBlock;
    int found = 0;

    while (*current != '\0') {
        // Case-insensitive search in the variable name
        // Tìm kiếm không phân biệt hoa thường trong tên biến
        char upper[4096];
        strncpy(upper, current, sizeof(upper) - 1);
        upper[sizeof(upper) - 1] = '\0';

        // Convert to uppercase for comparison (Chuyển thành chữ hoa để so sánh)
        for (char* c = upper; *c; c++) {
            if (*c >= 'a' && *c <= 'z') *c -= 32;
        }

        char upperPattern[256];
        strncpy(upperPattern, searchPattern, sizeof(upperPattern) - 1);
        upperPattern[sizeof(upperPattern) - 1] = '\0';
        for (char* c = upperPattern; *c; c++) {
            if (*c >= 'a' && *c <= 'z') *c -= 32;
        }

        if (strstr(upper, upperPattern) != NULL) {
            found++;
            printf("  [%d] %s\n", found, current);
        }

        current += strlen(current) + 1;
    }

    if (found == 0) {
        printf("  No variables matching \"%s\" found.\n", searchPattern);
        printf("  Khong tim thay bien nao khop voi \"%s\".\n", searchPattern);
    } else {
        printf("\n  Found %d variable(s) matching \"%s\".\n", found, searchPattern);
        printf("  Tim thay %d bien khop voi \"%s\".\n", found, searchPattern);
    }
    printf("\n");

    FreeEnvironmentStringsA(envBlock);
}

// ============================================================================
// Demo 4: Statistics about environment variables
// Minh họa 4: Thống kê về biến môi trường
// ============================================================================
void DemoStatistics() {
    printf("=== Demo 4: Environment Statistics ===\n");
    printf("=== Minh hoa 4: Thong Ke Moi Truong ===\n\n");

    LPCH envBlock = GetEnvironmentStringsA();
    if (envBlock == NULL) return;

    LPCH current = envBlock;
    int totalVars = 0;
    size_t totalNameLen = 0;
    size_t totalValueLen = 0;
    size_t maxNameLen = 0;
    size_t maxValueLen = 0;
    char longestName[256] = {0};
    char longestValueName[256] = {0};

    while (*current != '\0') {
        totalVars++;

        char* eq = strchr(current[0] == '=' ? current + 1 : current, '=');
        if (eq) {
            size_t nameLen = eq - current;
            size_t valueLen = strlen(eq + 1);

            totalNameLen += nameLen;
            totalValueLen += valueLen;

            if (nameLen > maxNameLen) {
                maxNameLen = nameLen;
                strncpy(longestName, current, nameLen);
                longestName[nameLen] = '\0';
            }
            if (valueLen > maxValueLen) {
                maxValueLen = valueLen;
                strncpy(longestValueName, current, nameLen < 255 ? nameLen : 255);
                longestValueName[nameLen < 255 ? nameLen : 255] = '\0';
            }
        }

        current += strlen(current) + 1;
    }

    printf("Statistics (Thong ke):\n");
    printf("  Total variables (Tong so bien)     : %d\n", totalVars);
    printf("  Avg name length (TB do dai ten)     : %.1f chars\n",
           totalVars > 0 ? (double)totalNameLen / totalVars : 0);
    printf("  Avg value length (TB do dai gia tri): %.1f chars\n",
           totalVars > 0 ? (double)totalValueLen / totalVars : 0);
    printf("  Longest name (Ten dai nhat)         : \"%s\" (%zu chars)\n",
           longestName, maxNameLen);
    printf("  Longest value (Gia tri dai nhat)    : variable \"%s\" (%zu chars)\n",
           longestValueName, maxValueLen);
    printf("\n");

    FreeEnvironmentStringsA(envBlock);
}

// ============================================================================
// Demo 5: Modify environment and verify block updates
// Minh họa 5: Thay đổi biến môi trường và xác nhận khối cập nhật
// ============================================================================
void DemoModifyAndVerify() {
    printf("=== Demo 5: Modify and Verify Block Updates ===\n");
    printf("=== Minh hoa 5: Thay Doi va Xac Nhan Cap Nhat ===\n\n");

    // Count variables before adding new ones
    // Đếm biến trước khi thêm mới
    LPCH envBlock = GetEnvironmentStringsA();
    int countBefore = 0;
    LPCH p = envBlock;
    while (*p != '\0') {
        countBefore++;
        p += strlen(p) + 1;
    }
    FreeEnvironmentStringsA(envBlock);

    printf("Variables before: %d (Bien truoc: %d)\n\n", countBefore, countBefore);

    // Add some variables (Thêm một số biến)
    printf("Adding 3 custom variables...\n");
    printf("Them 3 bien tuy chinh...\n");
    SetEnvironmentVariableA("DEMO_VAR_1", "First custom variable");
    SetEnvironmentVariableA("DEMO_VAR_2", "Second custom variable");
    SetEnvironmentVariableA("DEMO_VAR_3", "Third custom variable");

    // Count again - GetEnvironmentStrings returns a FRESH snapshot each time
    // Đếm lại - GetEnvironmentStrings trả về bản chụp MỚI mỗi lần gọi
    envBlock = GetEnvironmentStringsA();
    int countAfter = 0;
    p = envBlock;
    while (*p != '\0') {
        countAfter++;
        p += strlen(p) + 1;
    }
    FreeEnvironmentStringsA(envBlock);

    printf("Variables after: %d (Bien sau: %d)\n", countAfter, countAfter);
    printf("Difference: +%d variables\n\n", countAfter - countBefore);

    printf("NOTE: Each call to GetEnvironmentStrings() returns a new snapshot.\n");
    printf("GHI CHU: Moi lan goi GetEnvironmentStrings() tra ve ban chup moi.\n");
    printf("Previous snapshots are NOT updated automatically.\n");
    printf("Cac ban chup truoc do KHONG tu dong cap nhat.\n\n");

    // Clean up (Dọn dẹp)
    SetEnvironmentVariableA("DEMO_VAR_1", NULL);
    SetEnvironmentVariableA("DEMO_VAR_2", NULL);
    SetEnvironmentVariableA("DEMO_VAR_3", NULL);
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main() {
    printf("============================================================\n");
    printf("  Environment Block Explorer\n");
    printf("  Kham Pha Khoi Moi Truong\n");
    printf("  PID = %lu\n", GetCurrentProcessId());
    printf("============================================================\n\n");

    DemoRawBlock();
    DemoParseAll();
    DemoSearchVars("PATH");
    DemoSearchVars("USER");
    DemoSearchVars("PROGRAM");
    DemoStatistics();
    DemoModifyAndVerify();

    printf("============================================================\n");
    printf("  Exercise Complete! (Bai tap hoan tat!)\n");
    printf("============================================================\n");

    return 0;
}
